#pragma once

#include "growth.h"

#include <string>
#include <list>
#include <vector>

namespace GOB
{
	class Goal
	{
	public:
		Goal() = default;
		~Goal() = default;

		Goal(const std::string &name);

		// getter
		float get_value() const;
		float get_threshold() const;
		uint8_t get_rank() const;

		std::string get_name() const;

		// DISCONTENTMENT
		float get_discontentment(float new_val);
		float get_hierarchical_discontentment(float new_val);

		// FUTURE CHANGE
		// calculates by how much the goal would change after the given delta_current
		// e.g.: delta_current: 10s, current_time 0s, day_duration: 20s
		// -> get by how much the value would have changed 10s into the future (whole day is 20s)
		// (supports 1 day wrapping, e.g.:
		//  delta_current: 15s, current_time: 0s, day_duration: 10s -> OK, 5s into new day
		//  delta_current: 25s, current_time: 0s, day_duration: 10s -> NOT OK, 5s into second day!)
		float get_change(float delta_current, float daytime, float day_duration) const;

		// SETTER

		// clamps [0,1]
		void set_value(float val);
		// clamps [0,1]
		void set_threshold(float val);
		void set_rank(uint8_t val);

		// ETC
		bool exceeds_threshold() const;

		// adds val to value, clamps [0,1]
		void change_value(float val);

		void add_growth(const PermanentGrowth &growth);
		void add_growth(const RandomGrowth &growth);
		void add_growth(const TemporaryGrowth &growth);

		// update value based on growths
		void update(float delta, float current_time);

		// randomly regenerate change for all random growths
		void regenerate_random_growths();

		// remove all expired temporary growths
		void cleanup_temporary_growths();

	private:
		float m_value = 0.0f;
		float m_threshold = 0.0f;
		uint8_t m_rank = 1;

		std::string m_name = "NULL";

		// growth
		std::vector<PermanentGrowth> m_permanent_growths;
		std::vector<RandomGrowth> m_random_growths;
		std::list<TemporaryGrowth> m_temporary_growths;

		// helper
		float clamp(float val); // [0, 1]
	};
}
