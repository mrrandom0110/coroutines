#pragma once

#include <coroutine>

class [[nodiscard]] task {
public:

	struct promise_type;

	using handle_type = std::coroutine_handle<promise_type>;

	task(handle_type coro) : coro_(coro) {}

	task(const task&) = delete;

	task& operator=(const task&) = delete;

	task(task&& rhs) noexcept : coro_(rhs.coro_)
	{
		rhs.coro_ = nullptr;
	}

	task& operator=(task&& rhs) noexcept {
		if (&rhs == this)
			return *this;
		coro_ = rhs.coro_;
		rhs.coro_ = nullptr;
		return *this;
	}

	~task()
	{
		coro_.destroy();
	}

	struct cancellable
	{
		cancellable(bool flag = false) : flag_(flag) {}

		bool await_ready() const noexcept
		{
			return flag_;
		}

		void await_suspend(handle_type coro) const noexcept {}

		void await_resume() const noexcept {}

		bool flag_{false};
	};

	struct promise_type
	{
		bool is_cancelled{false};

		auto initial_suspend() noexcept
		{
			return std::suspend_always();
		}//YANDEX DATA BASE check

		auto final_suspend() noexcept
		{
			return std::suspend_always();
		}

		void unhandled_exception() noexcept
		{
			std::current_exception();
		}

		auto get_return_object() noexcept
		{
			return task{handle_type::from_promise(*this)};
		}

		auto await_transform(std::suspend_always) noexcept
		{
			if (is_cancelled) return cancellable{true};

			return cancellable{false};
		}

	};

public:

	bool resume() noexcept
	{
		coro_.resume();
	}

	void cancel() noexcept
	{
		if (coro_.done())
			return;

		set_cancel();

		coro_.resume();
	}

private:

	handle_type coro_;

private:

	void set_cancel() noexcept
	{
		coro_.promise().is_cancelled = true;
	}
};