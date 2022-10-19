#include "broadcaster.h"

#include <algorithm>

namespace GOB
{
	// getter
	Broadcaster &Broadcaster::get_instance()
	{
		static Broadcaster b;
		return b;
	}

	ACTION Broadcaster::add_action(const Action &action, const Attributes &attributes)
	{
		return add_action(std::make_shared<Action>(action), attributes);
	}

	// etc
	ACTION Broadcaster::add_action(ACTION action, const Attributes &attributes)
	{
		if (action == nullptr)
			return nullptr;
		m_actions.push_back({action, attributes});

		for (auto &entry : m_reasoners)
		{
			if (entry.attributes.contains(attributes) == false)
				continue;
			entry.reasoner->add_action(*action);
		}

		return action;
	}
	void Broadcaster::remove_action(ACTION action, const Attributes &attributes)
	{
		if (action == nullptr)
			return;

		auto itr = std::find_if(m_actions.begin(), m_actions.end(),
								[&](const ActionAttributes &entry)
								{ return entry.action == action; });

		if (itr != m_actions.end())
			m_actions.erase(itr);

		for (auto &entry : m_reasoners)
		{
			if (entry.attributes.contains(attributes) == false)
				continue;
			entry.reasoner->remove_action(action->get_id());
		}
	}

	void Broadcaster::add_reasoner(Reasoner *reasoner, const Attributes &attributes)
	{
		if (reasoner == nullptr)
			return;
		m_reasoners.push_back({reasoner, attributes});

		for (auto &entry : m_actions)
		{
			if (attributes.contains(entry.attributes) == false)
				continue;
			reasoner->add_action(*entry.action);
		}
	}

	void Broadcaster::remove_reasoner(Reasoner *reasoner)
	{
		if (reasoner == nullptr)
			return;

		auto itr = std::find_if(m_reasoners.begin(), m_reasoners.end(),
								[&](const ReasonerAttributes &entry)
								{ return entry.reasoner == reasoner; });

		if (itr != m_reasoners.end())
			m_reasoners.erase(itr);
	}
}
