#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <chrono>
#include <functional>
#include <thread>

class Timer
{
private:
	std::function<void(void)> func;

public:
	Timer(std::function<void(void)> func) : func(func) {}

	/**
	 * Run the given function every interval (in ms)
	 */
	void start(unsigned int interval, const bool &running) {
		while (running) {
			auto next = std::chrono::steady_clock::now() +
				std::chrono::milliseconds(interval);
			func();
			std::this_thread::sleep_until(next);
		}
	}

	/**
	 * Run the given function in a seperate thread every interval (in ms)
	 */
	std::thread startThread(unsigned int interval, const bool &running) {
		return std::thread(&Timer::start, this, interval, running);
	}
};

#endif
