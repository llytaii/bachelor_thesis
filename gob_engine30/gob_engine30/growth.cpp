#include "growth.h"

#include <random>

namespace GOB
{
	PermanentGrowth::PermanentGrowth(float change, float start, float end)
		: m_change_per_second{change / (end - start)}, m_start{start}, m_end{end}
	{
	}
	float PermanentGrowth::get_change(float delta, float current_time) const
	{
		float e = current_time;
		float s = current_time - delta;

		float change = 0.0f;
		bool s_inside = s >= m_start && s <= m_end;
		bool e_inside = e >= m_start && e <= m_end;

		change += (e - s) * m_change_per_second * (s_inside && e_inside);
		change += (m_end - s) * m_change_per_second * (s_inside && !e_inside);
		change += (e - m_start) * m_change_per_second * (!s_inside && e_inside);
		change += (m_end - m_start) * m_change_per_second * (!s_inside && !e_inside) * (s < m_start && e > m_end);

		return change;
	}

	TemporaryGrowth::TemporaryGrowth(float change, float duration)
		: m_change_per_second{change / duration}, duration{duration}
	{
	}
	void TemporaryGrowth::decrease_duration(float delta)
	{
		duration -= delta;
	}
	float TemporaryGrowth::get_change(float delta) const
	{
		delta = (delta > duration) ? duration : delta;
		return m_change_per_second * delta;
	}
	bool TemporaryGrowth::is_done() const
	{
		return (duration < 0.0f);
	}

	RandomGrowth::RandomGrowth(
		float change_start, float change_end,
		float start, float end)
		: m_change_start{change_start},
		  m_change_end{change_end}
	{
		m_start = start;
		m_end = end;

		regenerate_growth();
	}
	void RandomGrowth::regenerate_growth()
	{
		float change = get_random(m_change_start, m_change_end);
		m_change_per_second = change / (m_end - m_start);
	}
	float RandomGrowth::get_random(float start, float end)
	{
		std::random_device rd;
		std::mt19937_64 mt{rd()};
		std::uniform_real_distribution<float> dist{start, end};
		return dist(mt);
	}
}
