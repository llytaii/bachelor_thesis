#include "logger.h"

#include "action.h"
#include "goal.h"

#include <iomanip>
#include <limits>
#include <fstream>
#include <sstream>

namespace GOB
{
	void Logger::log(float timestamp, GOALS &goals, ACTIONS &actions, ACTION &current)
	{
		m_timestamps.push_back(timestamp);
		m_sources.push_back(" ");

		for (const auto &goal : goals)
		{
			if (goal == nullptr)
				continue;

			auto &goal_information = m_goals[goal->get_name()];
			goal_information.threshold = goal->get_threshold();
			goal_information.rank = goal->get_rank();

			auto &values = goal_information.values;
			while (values.size() < m_timestamps.size() - 1)
				values.push_back(std::numeric_limits<float>::quiet_NaN());
			values.push_back(goal->get_value());
		}

		for (const auto &action : actions)
		{
			if (action == nullptr)
				continue;

			auto &values = m_actions[action->get_name()];
			while (values.size() < m_timestamps.size() - 1)
				values.push_back(std::numeric_limits<float>::quiet_NaN());
			values.push_back(action->get_score());
		}

		auto current_name = (current) ? current->get_name() : "NULL";

		m_active_actions.push_back(current_name);
	}

	void Logger::log(float timestamp, GOALS &goals, ACTIONS &actions, ACTION &current, const std::string &source)
	{
		log(timestamp, goals, actions, current);
		m_sources[m_sources.size() - 1] = source;
	}

	void Logger::save_to_file(const std::string &filename)
	{
		std::fstream log{filename, std::ios::out | std::ios::app};

		calculate_action_progress();

		// scale goal values
		for (auto &goal_entry : m_goals)
			for (auto &val : goal_entry.second.values)
				val *= goal_entry.second.rank;

		// header
		log << "Time;Goals;";
		for (const auto &goal_entry : m_goals)
			log << goal_entry.first << ';';
		log << "Actions;";
		for (const auto &action_entry : m_actions)
			log << action_entry.first << ';';
		log << "Current;Source" << ';';
		for (const auto &action_entry : m_action_progress)
			log << action_entry.first << ';';

		log << std::endl;

		// ranks
		log << " ;Ranks;";
		for (const auto &entry : m_goals)
			log << static_cast<int>(entry.second.rank) << ';';
		log << std::endl;

		// thresholds
		log << " ;Thresholds;";
		for (const auto &entry : m_goals)
			log << entry.second.threshold << ';';
		log << std::endl;

		// logs
		for (size_t i = 0; i < m_timestamps.size(); ++i)
		{
			bool to_break = false;

			int hours = m_timestamps[i] / 60;
			int minutes = int(m_timestamps[i]) % 60;

			if (hours > 20)
				break;

			std::string shours = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
			std::string sminutes = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);

			log << shours << ':' << sminutes << ";";

			auto str = (m_sources[i].size() > 1) ? m_sources[i] : std::string{shours + ':' + sminutes};

			log << str;
			log << ";";

			// values
			for (const auto &entry : m_goals)
			{
				auto &vec = entry.second.values;
				float val = (i < vec.size()) ? vec[i] : std::numeric_limits<float>::quiet_NaN();
				log << val << ';';
			}
			log << " ;";

			// scores
			for (const auto &entry : m_actions)
			{
				auto &vec = entry.second;
				float val = (i < vec.size()) ? vec[i] : std::numeric_limits<float>::quiet_NaN();
				log << val << ';';
			}

			log << m_active_actions[i] << ';';
			log << m_sources[i] << ';';

			for (const auto &entry : m_action_progress)
			{
				auto &val = entry.second;
				log << val[i] << ';';
			}

			log << std::endl;
		}

		log.close();
		std::ifstream correct_dots{filename};
		std::stringstream buffer;
		buffer << correct_dots.rdbuf();
		correct_dots.close();
		auto str = buffer.str();
		for (auto &c : str)
			if (c == '.')
				c = '.';
		std::ofstream f{filename};
		f << str;
	}

	void Logger::calculate_action_progress()
	{
		for (auto &entry : m_actions)
		{
			auto &action = entry.first;

			for (int i = 0; i < m_sources.size(); ++i)
				m_action_progress[action].push_back(0.0f);

			std::vector<size_t> redeem_indicies;

			for (int i = 0; i < m_sources.size(); ++i)
				if (m_sources[i] == std::string{"redeem " + action})
					redeem_indicies.push_back(i);

			for (int i = 0, last = 0; i < redeem_indicies.size(); ++i)
			{
				int count = 0;
				float val = 0.0f;

				for (int j = last; j <= redeem_indicies[i]; ++j)
					if (m_active_actions[j] == action)
						++count;

				for (int j = last; j <= redeem_indicies[i]; ++j)
				{
					if (m_active_actions[j] == action)
					{
						if (j == redeem_indicies[i])
							m_action_progress[action][j] = 1.0f;
						else
							m_action_progress[action][j] = val;

						val += 1.0f / (count - 1);
					}
					else
					{
						m_action_progress[action][j] = val;
					}
				}

				last = ++redeem_indicies[i];
			}
		}

		for (auto &goal_entry : m_goals)
			for (auto &val : m_action_progress[goal_entry.first])
				val *= goal_entry.second.rank;
	}
}
