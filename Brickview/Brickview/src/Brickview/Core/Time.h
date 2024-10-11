#pragma once

namespace Brickview
{

	class DeltaTime
	{
	public:
		DeltaTime(float timeSeconds)
			: m_timeSeconds(timeSeconds) {}

		inline float getSeconds() const { return m_timeSeconds; }
		inline float getMilliseconds() const { return m_timeSeconds * 1000.0f; }

	private:
		float m_timeSeconds = 0.0f;
	};

	class Time
	{
	public:
		static float getTime();
	};

}