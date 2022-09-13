#pragma once

#include "MoveInstruction.h"

#include "gob_engine30/goal.h"
#include "gob_engine30/action.h"

using namespace Math;

// experimental, didnt work in some scenarios

class TalkInstruction
	: public MoveInstruction
{
	struct Communication
	{
		bool done = false;
		bool is_ignorable = false;

		bool brb = false;
		bool im_done = false;

		GOB::GOAL p_goal;
		GOB::ACTION p_action;

		size_t a_action;

		float duration;
	};

	enum Initiator
	{
		APPROACH,
		WAIT_ACCEPT,
		TALK,
		ABORT
	};

public:
	virtual ~TalkInstruction() = default;

	TalkInstruction(const TalkInstruction& other) = default;

	// initiator
	TalkInstruction(NodePtr komplize, float duration, float interruptable, uint8_t sequence_position = 0)
		: MoveInstruction{ sequence_position }
	{
		m_komplize = komplize;
		m_component_komplize = ComponentSystem::get()->getComponent<NPC>(m_komplize);

		if (m_component_komplize)
			m_reasoner_komplize = &m_component_komplize->m_reasoner;

		m_com = std::make_shared<Communication>();
		m_is_initiator = true;
		m_com->duration = duration;
		m_duration = duration;
		m_remaining_duration = duration;
		m_interruptable = interruptable;

		m_arrive_radius = 10.0f;
		m_state_initiator = Initiator::APPROACH;
	}

	// komplize
	TalkInstruction(NodePtr initiator, std::shared_ptr<Communication> com, uint8_t sequence_position = 0)
		: MoveInstruction{ sequence_position }
	{
		m_initiator = initiator;
		m_component_initiator = ComponentSystem::get()->getComponent<NPC>(m_initiator);

		if (m_component_initiator)
			m_reasoner_initiator = &m_component_initiator->m_reasoner;

		m_com = com;
		m_is_initiator = false;
	}

	GOB::ExecuteSignal execute_initiator()
	{
		m_com->is_ignorable = (m_remaining_duration / m_duration) > m_interruptable;

		switch (m_state_initiator)
		{
		case TalkInstruction::APPROACH:
			m_component->set_text("will reden");
			if (!MoveInstruction::approach(m_komplize->getWorldPosition())) return GOB::ExecuteSignal::CONTINUE;

			m_com->p_goal = std::make_shared<GOB::Goal>("talk back");
			m_com->p_goal->set_rank(2);
			m_com->p_goal->set_value(1.0f);

			m_reasoner_komplize->add_goal(*m_com->p_goal);

			m_com->p_action = std::make_shared<GOB::Action>("talk back");
			m_com->p_action->set_goal_change(m_com->p_goal->get_name(), -1.0f);
			m_com->p_action->add_instruction(TalkInstruction{ m_node, m_com }.clone());

			m_reasoner_komplize->add_action(*m_com->p_action);

			m_state_initiator = WAIT_ACCEPT;
			return GOB::ExecuteSignal::CONTINUE;
			break;

		case TalkInstruction::WAIT_ACCEPT:
			m_accept_wait_duration -= Game::getIFps();

			if (m_accept_wait_duration < 0.0f)
			{
				m_state_initiator = ABORT;
				return GOB::ExecuteSignal::CONTINUE;
			}

			if (m_reasoner_komplize->get_current_action() != nullptr)
			{
				if (m_reasoner_komplize->get_current_action()->get_id() == m_com->p_action->get_id())
				{
					m_state_initiator = TALK;
					return GOB::ExecuteSignal::CONTINUE;
				}
			}

			m_component->set_text(" waits");
			return GOB::ExecuteSignal::CONTINUE;
			break;

		case TalkInstruction::TALK:
			m_remaining_duration -= Game::getIFps();
			m_com->duration -= Game::getIFps();

			if (m_remaining_duration < 0.0f)
			{
				m_com->done = true;
				return GOB::ExecuteSignal::DONE;
			}

			// komplize interrupted
			if (m_reasoner_komplize->get_current_action() == nullptr || m_reasoner_komplize->get_current_action()->get_id() != m_com->p_action->get_id())
			{
				return GOB::ExecuteSignal::ABORT;
			}

			m_component->set_text(" plaudert");
			return GOB::ExecuteSignal::CONTINUE;
			break;

		case TalkInstruction::ABORT:
			m_reasoner_komplize->remove_goal(m_com->p_goal->get_name());
			m_reasoner_komplize->remove_action(m_com->p_action->get_id());

			return GOB::ExecuteSignal::ABORT;
			break;
		default:
			break;
		}
		return GOB::ExecuteSignal::CONTINUE;
	}

	GOB::ExecuteSignal execute_komplize()
	{
		if (m_com->done)
			return GOB::ExecuteSignal::DONE;

		// initiator interrupted
		if (m_reasoner_initiator->get_current_action() == nullptr || m_reasoner_initiator->get_current_action()->get_id() != m_com->a_action)
			return GOB::ExecuteSignal::ABORT;

		Vec3 orientation = m_initiator->getWorldPosition() - m_node->getWorldPosition();
		orientation.z = 0.0f;
		update_orientation(orientation.normalizeFast());

		m_component->set_text(" plaudert");
		return GOB::ExecuteSignal::CONTINUE;
	}

	virtual void init() override
	{
		if(m_is_initiator)
			m_com->a_action = m_reasoner->get_current_action()->get_id();
	}

	virtual void done() override
	{
		if (!m_is_initiator)
		{
			m_reasoner->remove_goal(m_com->p_goal->get_name());
			m_reasoner->remove_action(m_com->p_action->get_id());
		}
	}

	virtual void abort() override
	{
		done();
	}

	virtual void interrupt() override
	{
		if (!m_is_initiator)
		{
			m_reasoner->remove_action(m_com->p_action->get_id());
			m_reasoner->remove_goal(m_com->p_goal->get_name());
		}
		else
		{
			m_state_initiator = APPROACH;
		}
	}

	virtual GOB::ExecuteSignal execute() override
	{
		if (m_is_initiator)
			return execute_initiator();

		return execute_komplize();
	}

	virtual std::unique_ptr<Instruction> clone() const
	{
		return std::make_unique<TalkInstruction>(*this);
	}

	virtual float get_duration() const
	{
		return m_com->duration;
	}

	virtual bool is_ignorable()
	{
		return m_com->is_ignorable;
	}

private:

	// initiator
	NodePtr m_komplize;
	NPC* m_component_komplize = nullptr;
	GOB::Reasoner* m_reasoner_komplize = nullptr;

	float m_accept_wait_duration = 3.0f;

	float m_duration = 0.0f;
	float m_remaining_duration = 0.0f;

	Initiator m_state_initiator;
	float m_interruptable = 0.0f;

	// komplize
	NodePtr m_initiator;
	NPC* m_component_initiator = nullptr;
	GOB::Reasoner* m_reasoner_initiator = nullptr;

	// both
	std::shared_ptr<Communication> m_com;
	bool m_is_initiator = false;
};
