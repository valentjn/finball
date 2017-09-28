#ifndef GAME_COMPONENT_HPP_
#define GAME_COMPONENT_HPP_

#include <Timer.hpp>
#include <DoubleBuffer.hpp>

#include <functional>
#include <memory>
#include <tuple>
#include <thread>

namespace apply_detail
{
	template <class F, class Tuple, std::size_t... I>
	constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
	{
		return f(std::get<I>(std::forward<Tuple>(t))...);
	}
}  // namespace detail

template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t)
{
	return apply_detail::apply_impl(
		std::forward<F>(f), std::forward<Tuple>(t),
		std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

template<class Input>
class GameInputBase
{
	template<class C, class I, class O>
	friend class GameComponent;

protected:
	Input m_input;

public:
	const Input& getInput() const { return m_input; }

	virtual const Input& process() = 0;
};

template<class Input, class... Ingoings>
class GameInput : public GameInputBase<Input>
{
	std::tuple<const Ingoings&...> m_ingoings;

public:
	GameInput<Input, Ingoings...>(const Ingoings&... ingoings) : m_ingoings(std::forward_as_tuple(ingoings...)) {}

	const Input& process() final override
	{
		apply([this](const auto&... ingoings)
		{
			std::initializer_list<bool>{ ingoings.m_output.lock()... };
			this->m_input.process(ingoings.m_output.readBuffer()...);
			std::initializer_list<bool>{ ingoings.m_output.unlock()... };
		}, m_ingoings);
		return this->m_input;
	}
};

template<class Comp, class Input, class Output>
class GameComponent
{
	template<class I, class... C>
	friend class GameInput;

	Comp m_comp;
	DoubleBuffer<Output> m_output;
	std::thread m_thread;
	std::unique_ptr<GameInputBase<Input>> m_input;

public:
	template<class... Args>
	GameComponent<Comp, Input, Output>(Args&&... args) : m_comp(std::forward<Args>(args)...) {}

	~GameComponent<Comp, Input, Output>()
	{
		if (m_thread.joinable())
			m_thread.join();
	}

	template<class... Ingoings>
	void bindInput(const Ingoings&... comps)
	{
		m_input = std::make_unique<GameInput<Input, Ingoings...>>(comps...);
	}

	const Input& getInput() const { return m_input->getInput(); }
	const Output& getOutput() const { return m_output.readBuffer(); }
	const Comp& getComp() const { return m_comp; }
	Comp& getComp() { return m_comp; }

	void run(int ticks_per_second, const bool& running, bool main_thread = false)
	{
		auto fn = [&]()
		{
			assert(ticks_per_second >= 0);
			auto next = std::chrono::steady_clock::now();
			auto duration = std::chrono::microseconds(1000000 / ticks_per_second);
			while (running) {
				auto now = std::chrono::steady_clock::now();
				if (next > now)
					std::this_thread::sleep_until(next);
				else
					next = now;
				next += duration;

				if (m_input) {
					const Input& input = m_input->process();
					compute(input);
				}
				else {
					compute(Input{});
				}
			}
		};

		if (main_thread)
			fn();
		else
			m_thread = std::thread(fn);
	}

	//template<class T = Output, class = std::enable_if_t<std::is_same<void, T>::value>>
	//void compute(const Input& input)
	//{
	//	m_comp.compute(input);
	//}
	//
	//template<class T = Output, class = std::enable_if_t<!std::is_same<void, T>::value>>
	void compute(const Input& input)
	{
		m_comp.compute(input, m_output.writeBuffer());
		m_output.swap();
	}
};

#endif