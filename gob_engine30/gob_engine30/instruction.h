#pragma once

#include "common.h"

namespace GOB
{
	// instructions corresponding to the specific behavior of an action

	class Reasoner;

	enum class ExecuteSignal
	{
		CONTINUE,
		DONE,
		INTERRUPT,
		ABORT,
		ABORT_AND_COOLDOWN,
		MAX
	};

	class Instruction
	{

	public:
		/* INIT STAGE */
		Instruction() = default;
		virtual ~Instruction() = default; // make it polymorphic

		Instruction(uint8_t sequence_position);

		// polymorphism via unique_ptr
		virtual INSTRUCTION clone() const = 0;

		// critical

		// overloadable for custom conversion
		virtual void set_npc_reasoner(void* npc, Reasoner* reasoner);

		// getter
		uint8_t get_sequence_position() const;


		/* UPDATE STAGE */
		// gets executed once when instruction is set as active 
		virtual void init();

		// executed every frame when active
		virtual ExecuteSignal execute() = 0;

		// executed once, when action is interrupted/done/aborted
		virtual void done();
		virtual void interrupt();
		virtual void abort();

		// true if instruction is ignorable during interrupt
		virtual bool is_ignorable() const;

		// get approx. remaining duration 
		virtual float get_duration() const = 0;


	protected:
		void* m_npc = nullptr;
		Reasoner* m_reasoner = nullptr;

		uint8_t m_sequence_position = 0;
	};
}
