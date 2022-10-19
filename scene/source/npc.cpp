#include "npc.h"

#include "gob_engine30/attributes.h"
#include "gob_engine30/broadcaster.h"

#include <fstream>
#include <string>
#include <sstream>

REGISTER_COMPONENT(NPC)

using namespace Unigine;
using namespace Math;

void NPC::set_text(const std::string &activity)
{
	if (!m_text)
		return;
	std::string strategy;
	switch (m_reasoner.get_decision_strategy())
	{
	case GOB::DECISION::MOST_PRESSING:
		strategy = "mp";
		break;
	case GOB::DECISION::BEST_DISSATISFACTION:
		strategy = "bd";
		break;
	case GOB::DECISION::BEST_DISSATISFACTION_TIME:
		strategy = "bdt";
		break;
	case GOB::DECISION::HIERARCHICAL_MOST_PRESSING:
		strategy = "hmp";
		break;
	case GOB::DECISION::HIERARCHICAL_BEST_DISSATISFACION:
		strategy = "hbd";
		break;
	case GOB::DECISION::HIERARCHICAL_BEST_DISSATISFACION_TIME:
		strategy = "hbdt";
		break;
	case GOB::DECISION::HIERARCHICAL_MOST_PRESSING_BEST_DISSATISFACTION_TIME:
		strategy = "hmpbdt";
		break;
	case GOB::DECISION::UNKNOWN:
		break;
	default:
		break;
	}

	strategy += '\n';
	auto str = std::string{strategy + activity};
	m_text->setText(str.c_str());
}

void NPC::init()
{
	// read config file
	std::fstream file{"../gob/config.txt", std::ios::in};
	if (file.is_open())
	{
		std::string tmp;
		std::getline(file, tmp);

		std::stringstream ss{tmp};
		tmp.clear();

		m_active_geometry = false;

		while (ss >> tmp)
		{
			if (tmp == std::string{"mp"} && int(m_decision_strat) == 0)
				m_active_geometry = true;
			else if (tmp == std::string{"bd"} && int(m_decision_strat) == 1)
				m_active_geometry = true;
			else if (tmp == std::string{"bdt"} && int(m_decision_strat) == 2)
				m_active_geometry = true;
			else if (tmp == std::string{"hmp"} && int(m_decision_strat) == 3)
				m_active_geometry = true;
			else if (tmp == std::string{"hbd"} && int(m_decision_strat) == 4)
				m_active_geometry = true;
			else if (tmp == std::string{"hbdt"} && int(m_decision_strat) == 5)
				m_active_geometry = true;
			else if (tmp == std::string{"hmpbdt"} && int(m_decision_strat) == 6)
				m_active_geometry = true;
		}

		if (m_reasoner.get_id() > 7)
			m_active_geometry = true;

		// make geometry invisible
		if (!m_active_geometry)
		{
			auto &head = getNode()->getChild(0)->getChild(0)->getChild(0);
			head->setEnabled(0);

			auto &body = getNode()->getChild(0)->getChild(1)->getChild(0);
			body->setEnabled(0);
		}
	}

	// INIT

	NodePtr *n = &node;

	m_reasoner.set_npc(n);
	m_reasoner.set_day_duration(m_day_duration * 60);
	m_reasoner.set_decision_strategy(GOB::DECISION(int(m_decision_strat)));
	m_reasoner.set_evaluate_strategy(GOB::EVALUATE(int(m_evaluate_strat)));
	m_reasoner.set_redeem_strategy(GOB::REDEEM(int(m_evaluate_strat)));
	m_reasoner.set_current_time(m_current_time);

	GOB::Attributes attributes;
	for (int i = 0; i < m_attributes.size(); ++i)
	{
		auto attribute = std::string{m_attributes[i]};
		attributes.add(attribute);
	}

	GOB::Broadcaster::get_instance().add_reasoner(&m_reasoner, attributes);

	// goals
	for (int i = 0; i < m_goals.size(); ++i)
	{
		auto &prop_goal = m_goals[i];

		auto name = std::string{prop_goal->name};
		auto value = float(prop_goal->value);
		auto threshold = float(prop_goal->threshold);
		auto rank = uint8_t(prop_goal->rank);

		auto goal = GOB::Goal{name};

		goal.set_value(value);
		goal.set_threshold(threshold);
		goal.set_rank(rank);

		// growths

		auto &prop_perm_growths = prop_goal->m_permanent_growths;

		for (int i = 0; i < prop_perm_growths.size(); ++i)
		{
			float change = prop_perm_growths[i]->change;
			float start = prop_perm_growths[i]->start * 60;
			float end = prop_perm_growths[i]->end * 60;

			goal.add_growth({change, start, end});
		}

		auto &prop_rand_growths = prop_goal->m_random_growths;

		for (int i = 0; i < prop_rand_growths.size(); ++i)
		{
			float start = prop_rand_growths[i]->start;
			float end = prop_rand_growths[i]->end;

			float change_start = prop_rand_growths[i]->change_start;
			float change_end = prop_rand_growths[i]->change_end;

			goal.add_growth({change_start, change_end, start, end});
		}

		auto &prop_temp_growths = prop_goal->m_temporary_growths;

		for (int i = 0; i < prop_temp_growths.size(); ++i)
		{
			float change = prop_temp_growths[i]->change;
			float duration = prop_temp_growths[i]->duration;

			goal.add_growth({change, duration});
		}

		m_reasoner.add_goal(goal);
	}

	// text
	if (m_active_geometry)
	{
		m_text = ObjectText::create();

		m_text->setText("");
		m_text->setFontName("font.ttf");
		m_text->setFontSize(200);
		m_text->setFontResolution(100);

		m_text->setParent(this->getNode());
		m_text->translate(0.0f, 0.0f, 8.5f);
	}
}

void NPC::update()
{
	if (m_text)
		m_text->setText("");
	m_reasoner.update_state(Game::getIFps());
	m_reasoner.decide();
	m_reasoner.execute();
#ifdef GOB_ENABLE_LOGGING
	m_reasoner.log(Game::getIFps());
#endif // GOB_ENABLE_LOGGING
	if (!m_active_geometry)
		return;

	if (m_reasoner.get_current_action() == nullptr)
		set_text(" konnte keine Aktion finden");

	// text
	int hours = int(m_reasoner.get_current_time()) / 60;
	int minutes = int(m_reasoner.get_current_time()) % 60;
	hours = (hours > 23) ? hours - 24 : hours;

	std::string shours = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
	std::string sminutes = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);

	std::string prev_text = m_text->getText();
	auto text = std::string{shours + ":" + sminutes};
	prev_text = prev_text + '\n' + text;

	m_text->setText(prev_text.c_str());
	m_text->worldLookAt(Unigine::Game::getPlayer()->getPosition());
	m_text->rotate(270, 0, 180);
}

void NPC::shutdown()
{
	auto path = std::string{"../gob/logging/"};
	if (m_reasoner.get_id() < 10)
		m_reasoner.log(path);
	GOB::Broadcaster::get_instance().remove_reasoner(&m_reasoner);
}
