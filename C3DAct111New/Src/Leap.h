#pragma once
#include "Time.h"

template<typename T>
class Leap {
public:
	Leap() {
		maxTime = 0.0f;
		currentTime = 0.0f;
		m_output = nullptr;
	};
	void Start(T* output, const T& start, const T& end, float time) {
		m_output = output;
		m_start = start;
		m_end = end;
		maxTime = time;
		currentTime = 0.0f;
	};
	void Update()
	{
		if (currentTime >= maxTime)
			return;

		currentTime += Time::DeltaTime();
		if (currentTime > maxTime)
			currentTime = maxTime;
		*m_output = (m_end - m_start) * (currentTime / maxTime) + m_start;
	}

	bool IsFinish()
	{
		return currentTime >= maxTime;
	}
private:
	T m_start;
	T m_end;
	T* m_output;
	float maxTime;
	float currentTime;
};