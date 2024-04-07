#pragma once
#include <chrono>
#include <iostream>

class Timer
{
public:

	Timer() = default;
	Timer(float targetDuration, bool autoReset=false);

	
	bool infiniteLifetime() { return m_infiniteLifetime; }
	bool hasMetTargetDuration();
	std::chrono::duration<float> timeElapsed();
	std::chrono::duration<float> timeRemaining();
	float percentTargetMet();
	void reset();
	bool isSet() {
		return m_targetDuration > std::chrono::duration<float>{0};
	};
	bool isFirstTime();

	/*Timer(Timer&& other) noexcept : m_targetDuration(other.m_targetDuration)
	{
		m_targetDuration = other.m_targetDuration;
	}

	Timer(Timer& other) : m_targetDuration(other.m_targetDuration)
	{
		m_targetDuration = other.m_targetDuration;
	}

	Timer& operator=(Timer&& other) noexcept
	{
		
		std::swap(m_targetDuration, other.m_targetDuration);
		return *this;
	}

	Timer& operator=(const Timer& other)
	{
		m_targetDuration = other.m_targetDuration;
		return *this;
	}

	~Timer()
	{
	}*/
private:

	std::chrono::duration<float> m_targetDuration{};
	std::chrono::steady_clock::time_point m_timeSnapshot{ std::chrono::steady_clock::now() };
	bool m_infiniteLifetime{ false };
	bool m_autoReset{ false };
	bool m_firstTime{ true };

};

