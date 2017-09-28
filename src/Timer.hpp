#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <chrono>
#include <functional>
#include <thread>

class Timer
{
private:
	std::function<void(void)> func;
    std::chrono::steady_clock::time_point next;

public:
    template<class Fn>
	Timer(Fn func) : func(func), next(std::chrono::steady_clock::now()) {}

	/**
	 * Run the given function every interval (in ms)
	 */
	void start(int interval, const bool &running) {
        assert(interval >= 0);
		while (running) {
            std::this_thread::sleep_until(next);
            next += std::chrono::milliseconds(interval);

            if (next < std::chrono::steady_clock::now()) {
                next = std::chrono::steady_clock::now();
            }

			func();
		}
	}
};

#endif
