#include "action.h"
#include "goal.h"

namespace GOB
{
	// helper
	float round_score(float f)
	{
		return int(f * 1'000) / 1'000.0f;
	}

	size_t Action::s_id = 0;

	Action::Action()
	{
		m_id = s_id++;
	}

	Action::Action(const std::string &name)
		: Action{}
	{
		m_name = name;
	}

	Action::Action(const Action &other)
	{
		*this = other;
	}

	Action::Action(Action &&other) noexcept
	{
		*this = std::move(other);
	}

	Action &Action::operator=(const Action &other)
	{
		if (this != &other)
		{
			m_id = other.m_id;

			m_active_instruction = other.m_active_instruction;

			m_cooldown = other.m_cooldown;
			m_score = other.m_score;

			m_npc = other.m_npc;
			m_reasoner = other.m_reasoner;

			for (const auto &i : other.m_instructions)
				m_instructions.push_back(i->clone());

			m_name = other.m_name;
			m_full_cooldown = other.m_full_cooldown;

			for (const auto &i : other.m_template_instructions)
				m_template_instructions.push_back(i->clone());

			m_affected_goals = other.m_affected_goals;
		}
		return *this;
	}

	Action &Action::operator=(Action &&other) noexcept
	{
		if (this != &other)
		{
			m_id = other.m_id;

			m_active_instruction = other.m_active_instruction;

			m_cooldown = other.m_cooldown;
			m_score = other.m_score;

			m_npc = other.m_npc;
			m_reasoner = other.m_reasoner;

			m_instructions = std::move(other.m_instructions);

			m_name = std::move(other.m_name);
			m_full_cooldown = other.m_full_cooldown;

			m_template_instructions = std::move(m_template_instructions);
			m_affected_goals = std::move(other.m_affected_goals);
		}
		return *this;
	}

	// critical
	void Action::set_npc_reasoner(void *npc, Reasoner *reasoner)
	{
		for (auto &i : m_template_instructions)
			i->set_npc_reasoner(npc, reasoner);

		for (auto &i : m_instructions)
			i->set_npc_reasoner(npc, reasoner);

		m_npc = npc;
		m_reasoner = reasoner;
	}

	// getter
	size_t Action::get_id() const
	{
		return m_id;
	}

	float Action::get_cooldown() const
	{
		return m_cooldown;
	}

	float Action::get_duration() const
	{
		float duration = 0.0f;
		for (int i = m_active_instruction; i < m_instructions.size(); ++i)
			duration += m_instructions[i]->get_duration();
		return duration;
	}

	float Action::get_score() const
	{
		return m_score;
	}

	float Action::get_full_cooldown() const
	{
		return m_full_cooldown;
	}

	float Action::get_full_duration() const
	{
		float duration = 0.0f;
		for (auto &i : m_template_instructions)
			duration += i->get_duration();
		return duration;
	}

	std::string Action::get_name() const
	{
		return m_name;
	}

	void *Action::get_npc() const
	{
		return m_npc;
	}

	Reasoner *Action::get_reasoner() const
	{
		return m_reasoner;
	}

	CHANGE Action::get_goal_change_duration(const std::string &name) const
	{
		auto itr = m_affected_goals.find(name);
		if (itr != m_affected_goals.end())
			return itr->second;
		return {};
	}

	float Action::get_change(const std::string &name) const
	{
		return get_goal_change_duration(name).change;
	}

	float Action::get_change(const GOAL &goal) const
	{
		if (goal == nullptr)
			return 0.0f;
		return get_change(goal->get_name());
	}

	float Action::get_change(const Goal &goal) const
	{
		return get_change(goal.get_name());
	}

	const GOAL_CHANGE_MAP &Action::get_affected_goals() const
	{
		return m_affected_goals;
	}

	const INSTRUCTIONS &Action::get_instructions() const
	{
		return m_instructions;
	}

	// setter
	void Action::set_full_cooldown(float cooldown)
	{
		m_full_cooldown = cooldown;
	}

	void Action::set_on_cooldown()
	{
		m_cooldown = m_full_cooldown;
	}

	void Action::decrease_cooldown(float change)
	{
		m_cooldown -= change;
		m_cooldown = (m_cooldown > 0.0f) * m_cooldown;
	}

	void Action::reset_cooldown()
	{
		m_cooldown = 0.0f;
	}

	void Action::set_score(float score)
	{
		m_score = round_score(score);
	}

	void Action::set_name(const std::string &name)
	{
		m_name = name;
	}

	void Action::set_goal_change(const std::string &name, float change, float duration)
	{
		m_affected_goals[name] = {change, duration};
	}

	void Action::set_goal_change(const GOAL &goal, float change, float duration)
	{
		set_goal_change(goal->get_name(), change, duration);
	}

	void Action::set_goal_change(const Goal &goal, float change, float duration)
	{
		set_goal_change(goal.get_name(), change, duration);
	}

	/* UPDATE */
	// execution
	void Action::init()
	{
		if (m_instructions.size() == 0)
			return;
		m_instructions[m_active_instruction]->init();
	}

	ExecuteSignal Action::execute()
	{
		if (m_instructions.size() == 0)
			return ExecuteSignal::DONE;

		ExecuteSignal signal = m_instructions[m_active_instruction]->execute();

		if (signal == ExecuteSignal::DONE)
		{
			m_instructions[m_active_instruction]->done();
			if (advance_instruction() == false)
				reset_instructions();
			else
				signal = ExecuteSignal::CONTINUE;
		}

		return signal;
	}

	bool Action::advance_instruction()
	{
		++m_active_instruction;
		bool valid = m_active_instruction < m_instructions.size();
		m_active_instruction = valid * m_active_instruction + !valid * (m_instructions.size() - 1);
		if (valid)
			m_instructions[m_active_instruction]->init();
		return valid;
	}

	bool Action::interrupt()
	{
		if (m_instructions.size() == 0)
			return true;

		while (m_instructions[m_active_instruction]->is_ignorable())
		{
			m_instructions[m_active_instruction]->interrupt();
			if (!advance_instruction())
			{
				reset_instructions();
				return true;
			}
		}

		return false;
	}

	void Action::abort()
	{
		if (m_instructions.size() == 0)
			return;
		m_instructions[m_active_instruction]->abort();
		reset_instructions();
	}

	void Action::reset_instructions()
	{
		m_active_instruction = 0;

		m_instructions.clear();
		for (const auto &i : m_template_instructions)
			m_instructions.push_back(i->clone());
	}

	void Action::add_instruction(const INSTRUCTION &instruction)
	{
		if (instruction == nullptr)
			return;
		auto i = instruction->clone();
		i->set_npc_reasoner(m_npc, m_reasoner);
		m_template_instructions.push_back(std::move(i));
		std::sort(m_template_instructions.begin(), m_template_instructions.end(),
				  [](INSTRUCTION &first, INSTRUCTION &second)
				  { return first->get_sequence_position() < second->get_sequence_position(); });
		reset_instructions();
	}
}
