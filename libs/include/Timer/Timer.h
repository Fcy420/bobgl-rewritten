#ifndef TIMER_H
#define TIMER_H
#pragma once
#include <chrono>
#include<iostream>
class Timer
{
public:
	Timer();
	~Timer();
	void Reset();
	float Elapsed();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTimer;
};

Timer::Timer()
{
	Reset();
}
void Timer::Reset()
{
	startTimer = std::chrono::high_resolution_clock::now();
}

float Timer::Elapsed()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startTimer).count() * 0.001f * 0.001f * 0.001f;
}

#endif