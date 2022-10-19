#pragma once

#include "common.h"

#include <chrono>
#include <map>
#include <string>

namespace GOB
{
	// logger for the reasoners state

	class Goal;
	class Action;
	class Reasoner;

	// enable logging in reasoner.h
	// set path in save_to_file
	class Logger
	{
	public:
		// log data to memory
		void log(float timestamp, GOALS &goals, ACTIONS &actions, ACTION &current);

		// log data to memory
		void log(float timestamp, GOALS &goals, ACTIONS &actions, ACTION &current, const std::string &source);

		// save data to file
		void save_to_file(const std::string &filename);

		// calculate action progress
		void calculate_action_progress();

	private:
		// general information
		std::vector<float> m_timestamps;
		std::vector<std::string> m_sources;

		// goal information
		struct GoalInformation
		{
			float threshold;
			float rank;
			std::vector<float> values;
		};

		std::map<std::string, GoalInformation> m_goals;

		// action information
		std::map<std::string, std::vector<float>> m_actions;
		std::map<std::string, std::vector<float>> m_action_progress;

		// current/candidate information
		std::vector<std::string> m_active_actions;
	};
}
