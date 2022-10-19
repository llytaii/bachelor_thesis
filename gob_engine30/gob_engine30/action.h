#pragma once

#include "common.h"

#include "instruction.h"

#include <memory>
#include <vector>

namespace GOB
{
	class Reasoner;

	class Action
	{
	public:
		/* INIT */
		Action();
		~Action() = default;

		Action(const std::string &name);

		Action(const Action &other);
		Action(Action &&other) noexcept;

		Action &operator=(const Action &other);
		Action &operator=(Action &&other) noexcept;

		// critical
		// sets npc/reasoner in all instructions
		void set_npc_reasoner(void *npc, Reasoner *reasoner);

		// getter
		size_t get_id() const;

		// returns the remaining cooldown
		float get_cooldown() const;

		// returns the remaining duration
		float get_duration() const;

		// returns a score, indicating how useful this action is
		// set by the current strategy (the lower the better)
		float get_score() const;

		float get_full_cooldown() const;
		float get_full_duration() const;

		std::string get_name() const;

		void *get_npc() const;
		Reasoner *get_reasoner() const;

		// returns change: float and duration: float,
		// where the duration can be used to initialize a temporary growth
		// (see redeem strategies in reasoner)
		CHANGE get_goal_change_duration(const std::string &name) const;

		// returns change for a goal
		float get_change(const std::string &name) const;
		float get_change(const GOAL &goal) const;
		float get_change(const Goal &goal) const;

		const GOAL_CHANGE_MAP &get_affected_goals() const;
		const INSTRUCTIONS &get_instructions() const;

		// setter
		void set_full_cooldown(float cooldown); // sets duration of full_cooldown
		void set_on_cooldown();					// sets cooldown to full_cooldown
		void decrease_cooldown(float change);	// decreases cooldown
		void reset_cooldown();					// zeros cooldown

		void set_score(float score);

		void set_name(const std::string &name);

		void set_goal_change(const std::string &goal_name, float change, float duration = 0.0f);
		void set_goal_change(const GOAL &goal, float change, float duration = 0.0f);
		void set_goal_change(const Goal &goal, float change, float duration = 0.0f);

		/* UPDATE */

		// run once when chosen as active action
		void init();

		// run every tick, return status of active instruction, resets instructions if done
		ExecuteSignal execute();

		// false if no instruction left
		bool advance_instruction();

		// true if redeemable, calls interrupt on active instruction
		bool interrupt();

		// calls abort on active instruction, resets instructions
		void abort();

		void reset_instructions();

		void add_instruction(const INSTRUCTION &instruction); // max. 255 instructions!

	private:
		static size_t s_id;

		// state
		size_t m_id;

		uint8_t m_active_instruction = 0; // index of current instruction

		float m_cooldown = 0.0f;
		float m_score = 0.0f; // set by strategies, indicates how good an action is

		void *m_npc = nullptr;
		Reasoner *m_reasoner = nullptr;

		INSTRUCTIONS m_instructions;

		// data
		std::string m_name = "";
		float m_full_cooldown = 0.0f;

		INSTRUCTIONS m_template_instructions;
		GOAL_CHANGE_MAP m_affected_goals;
	};
}
