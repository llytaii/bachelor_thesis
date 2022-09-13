#pragma once

#include <random>

namespace GOB
{
	// growths informations for goals

	class PermanentGrowth
	{
	public:
		virtual ~PermanentGrowth() = default;

		// start/end in daytime seconds
		// e.g. day_duration = 100 s with 0s - 50s: day; 50s - 100s: night 
		// growth only at night -> start = 50, end = 100
		// change: how much should it grow in the given interval 
		PermanentGrowth(float change, float start, float end);

		float get_change(float delta, float current_time) const;

	protected:
		PermanentGrowth() = default;

		float m_change_per_second = 0.0f;
		float m_start = 0.0f;
		float m_end = 0.0f;
	};
	
	class TemporaryGrowth
	{
	public:
		TemporaryGrowth() = delete;

		// change: 1, duration : 10 -> after 10 s goals value grew by 1
		TemporaryGrowth(float change, float duration);

		void decrease_duration(float delta);

		// get change without decreasing duration
		float get_change(float delta) const;
		bool is_done() const;

	private:
		float m_change_per_second = 0.0f;
		float duration = 0.0f;
	};

	class RandomGrowth
		: public PermanentGrowth
	{
	public:
		RandomGrowth() = delete;

		// same as permanent growth but random change in [change_start, change_end[
		// random change is reseedable via regenerate_growth() (e.g. daily)
		RandomGrowth(float change_start, float change_end, float start, float end);

		// regenerates change in given interval using uniform distribution
		void regenerate_growth();

		float get_random(float start, float end); // [start, end[, uniform distribution
	private:
		float m_change_start;
		float m_change_end;
	};
}
