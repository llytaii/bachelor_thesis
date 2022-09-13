#include "reasoner.h"
#include "instruction.h"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace GOB
{

#ifdef GOB_ENABLE_LOGGING
	// initialize static id
	size_t Reasoner::s_id = 0;
#endif // GOB_ENABLE_LOGGING


	// rule of 5
	Reasoner::Reasoner()
	{
#ifdef GOB_ENABLE_LOGGING
		m_id = s_id++;
#endif // GOB_ENABLE_LOGGING
	}

	Reasoner::Reasoner(const Reasoner& other)
	{
		*this = other;
	}

	Reasoner::Reasoner(Reasoner&& other) noexcept
	{
		*this = std::move(other);
	}

	Reasoner& Reasoner::operator=(const Reasoner& other)
	{
		if (this != &other)
		{
#ifdef GOB_ENABLE_LOGGING
			m_id = s_id++;
			m_logger = other.m_logger;
#endif // GOB_ENABLE_LOGGING

			m_daytime = other.m_daytime;
			m_day_duration = other.m_day_duration;
			m_npc = other.m_npc;
			m_decision_strat = other.m_decision_strat;
			m_evaluate_strat = other.m_evaluate_strat;
			m_redeem_strat = other.m_redeem_strat;

			set_evaluate_strategy(m_evaluate_strat);
			set_redeem_strategy(m_redeem_strat);

			for (const auto& goal : other.m_goals)
			{
				if (goal == nullptr) continue;
				m_goals.push_back(std::make_shared<Goal>(*goal));
			}

			for (const auto& action : other.m_actions)
			{
				if (action == nullptr) continue;
				m_actions.push_back(std::make_shared<Action>(*action));
			}

			auto itr_current = std::find_if(m_actions.begin(), m_actions.end(),
				[other](const ACTION _action)
				{return _action->get_name() == other.get_current_action()->get_name(); });

			if (itr_current == m_actions.end())
				m_active_action = nullptr;
			else
				m_active_action = *itr_current;
		}
		return *this;
	}

	Reasoner& Reasoner::operator=(Reasoner&& other) noexcept
	{
		if (this != &other)
		{
#ifdef GOB_ENABLE_LOGGING
			m_id = s_id++;
			m_logger = other.m_logger;
#endif // GOB_ENABLE_LOGGING
			m_daytime = other.m_daytime;
			m_day_duration = other.m_day_duration;
			m_npc = other.m_npc;
			m_decision_strat = other.m_decision_strat;
			m_evaluate_strat = other.m_evaluate_strat;
			m_redeem_strat = other.m_redeem_strat;
			m_goals = std::move(other.m_goals);
			m_actions = std::move(other.m_actions);
			m_active_action = std::move(other.m_active_action);

			set_evaluate_strategy(m_evaluate_strat);
			set_redeem_strategy(m_redeem_strat);
		}
		return *this;
	}

	// getter

#ifdef GOB_ENABLE_LOGGING
	size_t Reasoner::get_id() const
	{
		return m_id;
	}
#endif // GOB_ENABLE_LOGGING

	void* Reasoner::get_npc() const
	{
		return m_npc;
	}
	float Reasoner::get_current_time() const
	{
		return m_daytime;
	}

	float Reasoner::get_day_duration() const
	{
		return m_day_duration;
	}
	DECISION Reasoner::get_decision_strategy() const
	{
		return m_decision_strat;
	}
	EVALUATE Reasoner::get_evaluate_strategy() const
	{
		return m_evaluate_strat;
	}
	REDEEM Reasoner::get_redeem_strategy() const
	{
		return m_redeem_strat;
	}
	ACTION Reasoner::get_current_action() const
	{
		return m_active_action;
	}

	// INIT
	void Reasoner::set_day_duration(float duration)
	{
		m_day_duration = duration;
	}

	void Reasoner::set_current_time(float time)
	{
		m_daytime = time;
	}

	void Reasoner::set_npc(void* npc)
	{
		for (auto& action : m_actions)
		{
			if (action == nullptr) continue;
			action->set_npc_reasoner(npc, this);
		}
		m_npc = npc;
	}
	void Reasoner::set_decision_strategy(DECISION strategy)
	{
		m_decision_strat = strategy;

		switch (strategy)
		{
		case GOB::DECISION::MOST_PRESSING:
			m_decision_func = std::bind(
				&Reasoner::most_pressing,
				this);
			break;

		case GOB::DECISION::BEST_DISSATISFACTION:
			m_decision_func = std::bind(
				&Reasoner::best_dissatisfaction,
				this);
			break;

		case GOB::DECISION::BEST_DISSATISFACTION_TIME:
			m_decision_func = std::bind(
				&Reasoner::best_dissatisfaction_time,
				this);
			break;

		case GOB::DECISION::HIERARCHICAL_MOST_PRESSING:
			m_decision_func = std::bind(
				&Reasoner::hierarchical_most_pressing,
				this);
			break;

		case GOB::DECISION::HIERARCHICAL_BEST_DISSATISFACION:
			m_decision_func = std::bind(
				&Reasoner::hierarchical_best_dissatisfaction,
				this);
			break;

		case GOB::DECISION::HIERARCHICAL_BEST_DISSATISFACION_TIME:
			m_decision_func = std::bind(
				&Reasoner::hierarchical_best_dissatisfaction_time,
				this);
			break;

		case GOB::DECISION::HIERARCHICAL_MOST_PRESSING_BEST_DISSATISFACTION_TIME:
			m_decision_func = std::bind(
				&Reasoner::hierarchical_most_pressing_best_dissatisfaction_time,
				this);
			break;

		case GOB::DECISION::UNKNOWN:
			break;

		default:
			break;
		}
	}

	void Reasoner::set_evaluate_strategy(EVALUATE strategy)
	{
		m_evaluate_strat = strategy;

		switch (strategy)
		{
		case GOB::EVALUATE::MINIMAL_SCORE:
			m_evaluate_func = std::bind(
				&Reasoner::minimal_score, this, std::placeholders::_1, std::placeholders::_2);
			break;

		case GOB::EVALUATE::MINIMAL_SCORE_TIME:
			m_evaluate_func = std::bind(
				&Reasoner::minimal_score_time, this, std::placeholders::_1, std::placeholders::_2);
			break;

		default:
			break;
		}
	}

	void Reasoner::set_redeem_strategy(REDEEM strategy)
	{
		m_redeem_strat = strategy;

		switch (strategy)
		{
		case GOB::REDEEM::INSTANT:
			m_redeem_func = std::bind(
				&Reasoner::redeem_instant, this, std::placeholders::_1);
			break;

		case GOB::REDEEM::DELAYED:
			m_redeem_func = std::bind(
				&Reasoner::redeem_delayed, this, std::placeholders::_1);
			break;

		default:
			break;
		}
	}

	// goals
	GOAL Reasoner::add_goal(const Goal& goal) {
		for (const auto& g : m_goals)
			if (g->get_name() == goal.get_name())
				return nullptr;

		auto g = std::make_shared<Goal>(goal);
		m_goals.push_back(g);
		std::sort(m_goals.begin(), m_goals.end(),
			[](const GOAL& first, const GOAL& second)
			{ return first->get_rank() > second->get_rank();  });
		return g;
	}

	GOAL Reasoner::get_goal(const std::string& name) {
		auto itr = std::find_if(m_goals.begin(), m_goals.end(),
			[&](const GOAL& goal)
			{ return goal->get_name() == name; });

		if (itr != m_goals.end())
			return *itr;
		return nullptr;
	}

	void Reasoner::remove_goal(const std::string& name)
	{
		auto itr = std::find_if(m_goals.begin(), m_goals.end(),
			[&](const GOAL& goal)
			{return goal->get_name() == name; });

		if (itr == m_goals.end()) return;

		m_goals.erase(itr);
		std::sort(m_goals.begin(), m_goals.end(),
			[](const GOAL& first, const GOAL& second)
			{ return first->get_rank() > second->get_rank();  });
	}

	const GOALS& Reasoner::get_goals() const
	{
		return m_goals;
	}


	// actions

	ACTION Reasoner::add_action(const Action& action)
	{
		auto itr = std::find_if(m_actions.begin(), m_actions.end(),
			[&](const ACTION& a) { return a->get_id() == action.get_id(); });
		if (itr != m_actions.end()) return nullptr;

		auto a = std::make_shared<Action>(action);
		a->set_npc_reasoner(m_npc, this);

		m_actions.push_back(a);
		return a;
	}

	void Reasoner::remove_action(size_t id)
	{
		auto itr = std::find_if(m_actions.begin(), m_actions.end(),
			[=](const ACTION& action)
			{ return action->get_id() == id; });

		if (itr == m_actions.end()) return;

		m_actions.erase(itr);
	}

	const ACTIONS& Reasoner::get_actions() const
	{
		return m_actions;
	}

	// testing
	void Reasoner::set_current_action(ACTION action)
	{
		m_active_action = action;
	}


	// UPDATE

	void Reasoner::update_state(float delta)
	{
		m_daytime += delta;

		// new day!
		if (m_daytime > m_day_duration)
		{
			m_daytime = m_daytime - m_day_duration;

			update_goals(delta - m_daytime, m_day_duration);
			update_actions(delta - m_daytime);

			delta = m_daytime;

			for (auto& goal : m_goals)
				goal->regenerate_random_growths();
		}

		update_goals(delta, m_daytime);
		update_actions(delta);
	}

	void Reasoner::decide()
	{
		ACTION candidate = m_decision_func();

		if (!candidate) return;

		if (m_active_action == nullptr)
		{
			m_active_action = candidate;
			m_active_action->init();
			return;
		}

		bool should_interrupt = m_evaluate_func(m_active_action, candidate);
		if (should_interrupt)
		{
			bool should_redeem = interrupt();
			if (should_redeem)
				m_redeem_func(m_active_action);

			m_active_action = candidate;
			m_active_action->init();
		}
	}

	void Reasoner::execute()
	{
		if (!m_active_action) return;

		ExecuteSignal signal = m_active_action->execute();

		switch (signal)
		{
		case GOB::ExecuteSignal::CONTINUE:
			return;

		case GOB::ExecuteSignal::DONE:
			if (!m_active_action) return;
			m_redeem_func(m_active_action);
			m_active_action->reset_instructions();
			m_active_action = nullptr;
			return;

		case GOB::ExecuteSignal::INTERRUPT:
			if (!m_active_action) return;
			if (interrupt())
				m_redeem_func(m_active_action);
			m_active_action = nullptr;
			return;

		case GOB::ExecuteSignal::ABORT:
			if (!m_active_action) return;
			abort_active_action();
			return;

		case GOB::ExecuteSignal::ABORT_AND_COOLDOWN:
			if (!m_active_action) return;
			abort_and_cooldown_active_action();
			return;

		case GOB::ExecuteSignal::MAX:
			return;
		default:
			break;
		}
	}

#ifdef GOB_ENABLE_LOGGING
	void Reasoner::log(float delta)
	{
		m_timestamp += delta;
		m_log_cooldown -= delta;
		if (m_log_cooldown <= 0.0f)
		{
			m_log_cooldown = 30.0f;
			m_logger.log(m_timestamp, m_goals, m_actions, m_active_action);
		}
	}
#endif // GOB_ENABLE_LOGGING

	void Reasoner::update_goals(const float delta, float current_daytime)
	{
		for (auto& goal : m_goals)
		{
			goal->update(delta, current_daytime);
			goal->cleanup_temporary_growths();
		}
	}

	void Reasoner::update_actions(float delta)
	{
		for (auto& action : m_actions)
			action->decrease_cooldown(delta);
	}


	ACTION Reasoner::most_pressing() {
		GOAL top_goal = std::make_shared<Goal>();
		for (auto& goal : m_goals)
			if (goal->exceeds_threshold() && goal->get_value() > top_goal->get_value())
				top_goal = goal;

		ACTION best_action;
		float best_score = 0;
		for (auto& action : m_actions) {
			// find smallest score 
			float score = action->get_change(top_goal);
			if (score < best_score) {
				best_action = action;
				best_score = score;
			}
			action->set_score(score);
		}

		return best_action;
	}

	ACTION Reasoner::best_dissatisfaction() {
		ACTION best_action = nullptr;
		float best_score = std::numeric_limits<float>::infinity();
		for (const auto& action : m_actions) {
			// total discontentment (score) for current action
			float score = 0;
			for (const auto& goal : m_goals) {
				// calculate the new value after the action
				float new_value = goal->get_value() + action->get_change(goal);
				bool exceed_threshold = new_value > goal->get_threshold();
				// get discontentment of the new value
				score += exceed_threshold * goal->get_discontentment(new_value);
			}
			action->set_score(score);
			if (score < best_score) {
				best_action = action;
				best_score = score;
			}
		}
		return best_action;
	}

	ACTION Reasoner::best_dissatisfaction_time() {
		ACTION best_action = nullptr;
		float best_score = std::numeric_limits<float>::infinity();

		for (const auto& action : m_actions) {
			// total discontentment (score) for current action
			float score = 0;
			for (const auto& goal : m_goals) {
				// calculate the new value after the action
				float new_value = goal->get_value() + action->get_change(goal);

				// calculate the change due to time alone
				// this is a bit different from millington, 
				// since the exact growth is taken into account
				// not average growth per tick
				new_value += goal->get_change(action->get_duration(), m_daytime, m_day_duration);

				bool exceed_threshold = new_value > goal->get_threshold();

				// get discontentment of the new value
				score += exceed_threshold * goal->get_discontentment(new_value);
			}
			action->set_score(score);
			if (score < best_score) {
				best_action = action;
				best_score = score;
			}
		}
		return best_action;
	}

	ACTION Reasoner::hierarchical_most_pressing()
	{
		GOAL top_goal = std::make_shared<Goal>();

		for (const auto& goal : m_goals)
		{
			float val = goal->exceeds_threshold() * goal->get_rank() * goal->get_value();
			float top_val = top_goal->get_value() * top_goal->get_rank();

			if (val > top_val)
				top_goal = goal;
		}

		ACTION best_action;
		float best_score = 0;

		for (const auto& action : m_actions)
		{
			float score = action->get_change(top_goal);

			if (score < best_score)
			{
				best_action = action;
				best_score = score;
			}

			action->set_score(score);
		}

		return best_action;
	}

	ACTION Reasoner::hierarchical_best_dissatisfaction()
	{
		ACTION best_action = nullptr;
		float best_score = std::numeric_limits<float>::infinity();

		for (const auto& action : m_actions)
		{
			// total discontentment (score) for current action
			float score = 0;
			for (const auto& goal : m_goals)
			{
				// calculate the new value after the action
				float new_value = goal->get_value() + action->get_change(goal);

				bool valid_threshold = new_value > goal->get_threshold();

				// get discontentment of the new value (hierarchically) 
				score += valid_threshold * goal->get_hierarchical_discontentment(new_value);
			}

			if (score < best_score)
			{
				best_action = action;
				best_score = score;
			}

			action->set_score(score);
		}

		return best_action;
	}

	ACTION Reasoner::hierarchical_best_dissatisfaction_time()
	{
		ACTION best_action = nullptr;
		float best_score = std::numeric_limits<float>::infinity();

		for (const auto& action : m_actions)
		{
			// total discontentment (score) for current action
			float score = 0;
			for (const auto& goal : m_goals)
			{
				// calculate the new value after the action
				float new_value = goal->get_value() + action->get_change(goal);

				// calculate the change due to time alone
				// this is a bit different from millington, 
				// since the exact growth is taken into account
				// not average growth per tick
				new_value += goal->get_change(action->get_duration(), m_daytime, m_day_duration);

				bool valid_threshold = new_value > goal->get_threshold();

				// get discontentment of the new value (hierarchically)
				score += valid_threshold * goal->get_hierarchical_discontentment(new_value);
			}

			if (score < best_score)
			{
				best_action = action;
				best_score = score;
			}

			action->set_score(score);
		}

		return best_action;
	}

	ACTION Reasoner::hierarchical_most_pressing_best_dissatisfaction_time() {
		GOAL top_goal = std::make_shared<Goal>();
		for (const auto& goal : m_goals)
		{
			// hierarchicallly weighted value
			float val = goal->get_value() * goal->get_rank() * goal->exceeds_threshold();
			float top_val = top_goal->get_value() * top_goal->get_rank();

			if (val > top_val)
				top_goal = goal;
		}

		GOALS top_goals;
		for (const auto& goal : m_goals)
			if (goal->exceeds_threshold() && goal->get_rank() >= top_goal->get_rank())
				top_goals.push_back(goal);


		ACTIONS best_actions;
		for (const auto& action : m_actions)
			for (const auto& goal : top_goals)
				if (action->get_change(goal) < 0.0f)
				{
					best_actions.push_back(action);
					break;
				}

		for (auto& action : m_actions)
			action->set_score(10'000.0f);

		ACTION best_action = nullptr;
		float best_score = std::numeric_limits<float>::infinity();
		for (const auto& action : best_actions) {
			// total discontentment (score) for current action
			float score = 0;
			for (const auto& goal : m_goals) {
				// calculate the new value after the action
				float new_value = goal->get_value() + action->get_change(goal);

				bool exceed_threshold = new_value > goal->get_threshold();
				// get discontentment of the new value (hierarchically)
				score += exceed_threshold * goal->get_hierarchical_discontentment(new_value);
			}

			score += std::log(action->get_duration() / 60);

			if (score < best_score) {
				best_action = action;
				best_score = score;
			}

			action->set_score(score);
		}
		return best_action;
	}

	/* EVALUATE */
	bool Reasoner::minimal_score(ACTION current, ACTION candidate)
	{
		if (current == candidate) return false;

		return (candidate->get_score() < current->get_score());
	}

	bool Reasoner::minimal_score_time(ACTION current, ACTION candidate)
	{
		if (current == candidate) return false;

		float score_diff = current->get_score() / candidate->get_score();
		float time_diff = candidate->get_duration() / current->get_duration();

		return (score_diff > time_diff);
	}

	/* INTERRUPT */
	bool Reasoner::interrupt()
	{
		bool should_redeem = m_active_action->interrupt();
#ifdef GOB_ENABLE_LOGGING
		m_logger.log(m_timestamp, m_goals, m_actions, m_active_action, "interrupt " + m_active_action->get_name());
#endif // GOB_ENABLE_LOGGING
		return should_redeem;
	}

	/* REDEEM */
	void Reasoner::redeem_instant(ACTION action)
	{
		for (const auto& pair : action->get_affected_goals())
		{
			auto& change = pair.second;
			auto goal = get_goal(pair.first);
			if (goal != nullptr)
				goal->change_value(change.change);
		}

#ifdef GOB_ENABLE_LOGGING
		m_logger.log(m_timestamp, m_goals, m_actions, m_active_action, "redeem " + action->get_name());
#endif // GOB_ENABLE_LOGGING
	}

	void Reasoner::redeem_delayed(ACTION action)
	{
		for (const auto& pair : action->get_affected_goals())
		{
			auto goal = get_goal(pair.first);
			if (goal != nullptr)
			{
				auto& change = pair.second;
				if (change.duration > 0.0f)
					goal->add_growth({ change.change, change.duration });
				else
					goal->change_value(change.change);
			}
		}
#ifdef GOB_ENABLE_LOGGING
		m_logger.log(m_timestamp, m_goals, m_actions, m_active_action, "redeem " + action->get_name());
#endif // GOB_ENABLE_LOGGING
	}

	/* ETC */
	void Reasoner::abort_active_action()
	{
		m_active_action->abort();
		m_active_action = nullptr;
	}
	void Reasoner::abort_and_cooldown_active_action()
	{
		m_active_action->set_on_cooldown();
		abort_active_action();
	}

#ifdef GOB_ENABLE_LOGGING
	void Reasoner::log(const std::string& path)
	{
		std::string filename = path;
		filename += "reasoner_" + std::to_string(m_id) + "_strategy_";
		switch (m_decision_strat)
		{
		case GOB::DECISION::MOST_PRESSING:
			filename += "most_pressing";
			break;
		case GOB::DECISION::BEST_DISSATISFACTION:
			filename += "best_dissatisfaction";
			break;
		case GOB::DECISION::BEST_DISSATISFACTION_TIME:
			filename += "best_dissatisfaction_time";
			break;
		case GOB::DECISION::HIERARCHICAL_MOST_PRESSING:
			filename += "hierarchical_most_pressing";
			break;
		case GOB::DECISION::HIERARCHICAL_BEST_DISSATISFACION:
			filename += "hierarchical_best_dissatisfaction";
			break;
		case GOB::DECISION::HIERARCHICAL_BEST_DISSATISFACION_TIME:
			filename += "hierarchical_best_dissatisfaction_time";
			break;
		case GOB::DECISION::HIERARCHICAL_MOST_PRESSING_BEST_DISSATISFACTION_TIME:
			filename += "hierarchical_most_pressing_best_dissatisfaction_time";
			break;
		case GOB::DECISION::UNKNOWN:
			break;
		default:
			break;
		}
		filename += "_log.txt";
		m_logger.save_to_file(filename);
	}
#endif // GOB_ENABLE_LOGGING
}

