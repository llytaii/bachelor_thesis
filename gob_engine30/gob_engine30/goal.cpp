#include "goal.h"

namespace GOB
{
	Goal::Goal(const std::string& name)
		: m_name{ name }
	{}

	// GETTER
	float Goal::get_value() const 
	{
		return m_value;
	}
	float Goal::get_threshold() const 
	{
		return m_threshold;
	}
	uint8_t Goal::get_rank() const
	{
		return m_rank;
	}
	bool Goal::exceeds_threshold() const
	{
		return m_value > m_threshold;
	}
	std::string Goal::get_name() const
	{
		return m_name;
	}

	// DISCONTENTMENT
	float Goal::get_discontentment(float new_val) 
	{
		new_val = clamp(new_val);
		return new_val * new_val;
	}

	float Goal::get_hierarchical_discontentment(float new_val)
	{
		new_val = clamp(new_val);
		// rank^2 + 1 * value ^ 4 
		//return (m_rank * m_rank + 1) * new_val * new_val * new_val * new_val;
		return m_rank * new_val * new_val;
	}

	// FUTURE CHANGE
	float Goal::get_change(float delta_current, float current_time, float day_duration) const
	{
		// duration expands into next day? (max 1 day overflow)
		bool day_overflow = (delta_current + current_time) > day_duration;

		// if so: duration0 is remaining duration of this day
		float remaining = day_overflow * (day_duration - current_time);
		remaining += !day_overflow * delta_current;

		// if so: duration1 is duration within next day
		float next = day_overflow * (current_time + delta_current - day_duration);

		float change = 0.0f;

		// change day0
		for (const auto& growth : m_permanent_growths)
			change += growth.get_change(remaining, current_time + remaining);

		for (const auto& growth : m_random_growths)
			change += growth.get_change(remaining, current_time + remaining);

		// change day1
		for (const auto& growth : m_permanent_growths)
			change += growth.get_change(next, next);

		for (const auto& growth : m_random_growths)
			change += growth.get_change(next, next);

		// temporary
		for (const auto& growth : m_temporary_growths)
			change += growth.get_change(delta_current);

		return change;
	}

	// SETTER 
	void Goal::set_value(const float insistance) 
	{
		m_value = clamp(insistance);
	}
	void Goal::set_threshold(const float threshold)
	{
		m_threshold = clamp(threshold);
	}
	void Goal::set_rank(uint8_t rank)
	{
		m_rank = rank;
	}
	

	// ETC
	void Goal::change_value(const float change)
	{
		m_value = clamp(m_value + change);
	}

	void Goal::add_growth(const PermanentGrowth& growth)
	{
		m_permanent_growths.push_back(growth);
	}
	void Goal::add_growth(const RandomGrowth& growth)
	{
		m_random_growths.push_back(growth);
	}
	void Goal::add_growth(const TemporaryGrowth& growth)
	{
		m_temporary_growths.push_back(growth);
	}

	void Goal::update(float delta, float current_time)
	{
		float change = 0.0f;

		for(auto& growth : m_permanent_growths)
			change += growth.get_change(delta, current_time);

		for(auto& growth : m_random_growths)
			change += growth.get_change(delta, current_time);

		for (auto& growth : m_temporary_growths)
		{
			change += growth.get_change(delta);
			growth.decrease_duration(delta);
		}

		change_value(change);
	}

	void Goal::regenerate_random_growths()
	{
		for (auto& growth : m_random_growths)
			growth.regenerate_growth();
	}

	void Goal::cleanup_temporary_growths()
	{
		// erase all expired growths within the !list!
		for (auto itr = m_temporary_growths.cbegin(); itr != m_temporary_growths.cend();)
		{
			if (itr->is_done())
				itr = m_temporary_growths.erase(itr);
			else
				++itr;
		}
	}



	// helper
	float Goal::clamp(float val) {
		if (val < 0.0f) val = 0.0f;
		if (val > 1.0f) val = 1.0f;
		return val;
	}
}

