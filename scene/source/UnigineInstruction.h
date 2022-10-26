#pragma once

#include "gob_engine30/instruction.h"

#include "npc.h"

#include "Unigine.h"

// parent class of all instructions, converts the void* npc to Unigine::NodePtr

class GOB::Reasoner;
class UnigineInstruction
	: public GOB::Instruction
{
public:
	UnigineInstruction() = default;
	virtual ~UnigineInstruction() = default;

	UnigineInstruction(uint8_t _sequence_order)
		: Instruction{_sequence_order}
	{
	}

	UnigineInstruction(const UnigineInstruction &other) = default;
	UnigineInstruction(UnigineInstruction &&other) = default;

	virtual void set_npc_reasoner(void *npc, GOB::Reasoner *reasoner) override
	{
		if (!npc)
			return;
		m_node = *static_cast<NodePtr *>(npc);
		m_component = ComponentSystem::get()->getComponent<NPC>(m_node);

		m_npc = npc;
		m_reasoner = reasoner;
	}

	// interface
	virtual GOB::ExecuteSignal execute() = 0;
	virtual std::unique_ptr<Instruction> clone() const = 0;
	virtual float get_duration() const = 0;
	virtual bool is_ignorable() = 0;

protected:
	Unigine::NodePtr m_node;
	NPC *m_component = nullptr;
};
