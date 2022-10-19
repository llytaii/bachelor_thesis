#include "instruction.h"

namespace GOB
{
	Instruction::Instruction(uint8_t sequence_position)
		: m_sequence_position{sequence_position}
	{
	}

	// critical
	void Instruction::set_npc_reasoner(void *npc, Reasoner *reasoner)
	{
		m_npc = npc;
		m_reasoner = reasoner;
	}

	// getter
	uint8_t Instruction::get_sequence_position() const
	{
		return m_sequence_position;
	}

	/* UPDATE */
	void Instruction::init()
	{
	}

	void Instruction::done()
	{
	}
	void Instruction::interrupt()
	{
	}
	void Instruction::abort()
	{
	}

	bool Instruction::is_ignorable() const
	{
		return false;
	}
}
