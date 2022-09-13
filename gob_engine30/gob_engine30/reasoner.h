#pragma once

#include "common.h"

#include "action.h"
#include "goal.h"

#ifdef GOB_ENABLE_LOGGING
#include "logger.h"
#endif // GOB_ENABLE_LOGGING

#include <functional>
#include <memory>
#include <vector>

namespace GOB
{
	// brain of the npc


	// types of predefined strategies for...
	enum class DECISION // decide on the best action (candidate action)
	{
		// standard millington
		MOST_PRESSING,
		BEST_DISSATISFACTION,
		BEST_DISSATISFACTION_TIME,

		// hierarchical strategies
		HIERARCHICAL_MOST_PRESSING,
		HIERARCHICAL_BEST_DISSATISFACION,
		HIERARCHICAL_BEST_DISSATISFACION_TIME,

		HIERARCHICAL_MOST_PRESSING_BEST_DISSATISFACTION_TIME,

		UNKNOWN
	};

	enum class EVALUATE // evaluate whether the candidate should replace the current action or not
	{
		MINIMAL_SCORE, // action with lower score wins
		MINIMAL_SCORE_TIME, // lower score vs remaining duration 
		UNKNOWN
	};

	enum class REDEEM // if an action is done, how to redeem it
	{
		DELAYED,
		INSTANT,
		UNKOWN
	};

	class Broadcaster;

	class Reasoner
	{
	public:
		// rule of 5
		Reasoner();
		~Reasoner() = default;

		Reasoner(const Reasoner& other);
		Reasoner(Reasoner&& other) noexcept;

		Reasoner& operator=(const Reasoner& other);
		Reasoner& operator=(Reasoner&& other) noexcept;

		// GETTER 

		// logging
#ifdef GOB_ENABLE_LOGGING
		size_t get_id() const;
#endif // GOB_ENABLE_LOGGING

		void* get_npc() const;

		float get_current_time() const;
		float get_day_duration() const;

		DECISION get_decision_strategy() const;
		EVALUATE get_evaluate_strategy() const;
		REDEEM get_redeem_strategy() const;

		ACTION get_current_action() const;

		// INIT 
		void set_day_duration(float duration);
		void set_current_time(float time);

		void set_npc(void* npc);

		void set_decision_strategy(DECISION strategy);
		void set_evaluate_strategy(EVALUATE strategy);
		void set_redeem_strategy(REDEEM strategy);

		// goals

		// copies the given goal and returns a handle
		// !nullptr if name is not unique!
		GOAL add_goal(const Goal& goal);

		// get a handle to a goal with the given name
		// !nullptr if name not found!
		GOAL get_goal(const std::string& name);

		// remove goal by name
		void remove_goal(const std::string& name);

		const GOALS& get_goals() const;

		// actions
		// copies the given action and returns a handle
		// !returns nullptr if id is not unique!
		ACTION add_action(const Action& action);

		// delete action by id
		void remove_action(size_t id);

		const ACTIONS& get_actions() const;

		// testing
		void set_current_action(ACTION action);

		/* UPDATE */

		// update goal-values and action-cooldowns
		void update_state(float delta);

		// choose the best action, interrupt active one
		void decide();

		// execute chosen action
		void execute();

#ifdef GOB_ENABLE_LOGGING
		void log(float delta);
#endif // GOB_ENABLE_LOGGING


		void update_goals(float delta, float current_daytime);
		void update_actions(float delta);

		// standard millington (+ threshold support)
		ACTION most_pressing();
		ACTION best_dissatisfaction();
		ACTION best_dissatisfaction_time();

		// + hierarchical
		ACTION hierarchical_most_pressing();
		ACTION hierarchical_best_dissatisfaction();
		ACTION hierarchical_best_dissatisfaction_time();

		ACTION hierarchical_most_pressing_best_dissatisfaction_time();

		/* EVALUATE */
		// returns true if current should be interrupted 
		bool minimal_score(ACTION current, ACTION candidate);
		bool minimal_score_time(ACTION current, ACTION candidate);

		/* INTERRUPT */
		// interrupts active action
		// returns true if active action is redeemable
		bool interrupt();

		/* REDEEM */
		void redeem_instant(ACTION action);
		void redeem_delayed(ACTION action);

		/* ETC */
		void abort_active_action();
		void abort_and_cooldown_active_action();

#ifdef GOB_ENABLE_LOGGING
		void log(const std::string& path);
#endif // GOB_ENABLE_LOGGING

	private:
#ifdef GOB_ENABLE_LOGGING
		static size_t s_id;

		// logging
		size_t m_id;
		float m_timestamp = 0.0f;
		float m_log_cooldown = 0.0f;
		Logger m_logger;
#endif // GOB_ENABLE_LOGGING

		// information
		float m_daytime = 0.0f;
		float m_day_duration = 60 * 24; // 1 min = 1 h 

		void* m_npc = nullptr;

		GOALS m_goals;
		ACTIONS m_actions;

		ACTION m_active_action;

		// configuration
		DECISION m_decision_strat = DECISION::BEST_DISSATISFACTION;
		std::function<ACTION()> m_decision_func = std::bind(
			&Reasoner::best_dissatisfaction, this);

		EVALUATE m_evaluate_strat = EVALUATE::MINIMAL_SCORE;
		std::function<bool(ACTION, ACTION)>m_evaluate_func = std::bind(
			&Reasoner::minimal_score, this, std::placeholders::_1, std::placeholders::_2);

		REDEEM m_redeem_strat = REDEEM::INSTANT;
		std::function<void(ACTION)> m_redeem_func = std::bind(
			&Reasoner::redeem_instant, this, std::placeholders::_1);
	};
}

