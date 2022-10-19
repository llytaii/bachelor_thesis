#pragma once

#include "MoveInstruction.h"

// NPC moves somewhere and "does" something there

class DoInstruction
	: public MoveInstruction
{
public:
	DoInstruction(const std::string &task,
				  NodePtr location,
				  uint8_t sequence_position,
				  float duration,
				  float interrupt_progress)
		: m_task{task},
		  duration{duration},
		  m_remaining_duration{duration},
		  m_interrupt_progress{interrupt_progress},
		  MoveInstruction{sequence_position, location, 0.1f, true}
	{
	}

	virtual float get_duration() const override
	{
		return MoveInstruction::get_duration() + duration;
	}

	virtual GOB::ExecuteSignal execute() override
	{
		if (MoveInstruction::execute() == GOB::ExecuteSignal::DONE)
		{
			Vec3 direction = m_destination_node->getChild(0)->getWorldPosition() - m_node->getWorldPosition();
			direction.z = 0.0f;
			direction.normalizeFast();
			update_orientation(direction);

			m_remaining_duration -= Game::getIFps();

			// set npc text
			m_component->set_text(m_task);

			return GOB::ExecuteSignal(m_remaining_duration < 0);
		}
		return GOB::ExecuteSignal::CONTINUE;
	}

	virtual bool is_ignorable() override
	{
		return ((duration - m_remaining_duration) / duration) > m_interrupt_progress;
	}

	virtual std::unique_ptr<GOB::Instruction> clone() const override
	{
		return std::make_unique<DoInstruction>(*this);
	};

private:
	std::string m_task;
	float duration;
	float m_remaining_duration;
	float m_interrupt_progress;
};
