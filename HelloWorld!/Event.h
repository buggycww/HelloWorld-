#pragma once

#include <functional>
#include <vector>
#include <memory>

// move only RAII token. owning it keeps the subscription alive
// destroying it (or calling Reset) unsubscribes
class Subscription
{
public:
	Subscription() = default;
	~Subscription() { Reset(); }

	Subscription(Subscription&& other) noexcept : kill_(std::move(other.kill_)) {}
	Subscription& operator=(Subscription&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			kill_ = std::move(other.kill_);
		}

		return *this;
	}

	void Reset()
	{
		if (kill_) 
		{ 
			kill_();  kill_ = nullptr; 
		}
	}

private:
	template <typename... Args> friend class Event;
	explicit Subscription(std::function<void()>&& kill) : kill_(std::move(kill)) {}

	std::function<void()> kill_;
};

template <typename... Args>
class Event
{
public:
	using Callback = std::function<void(Args...)>;

	Event() = default;
	// copying a publisher would duplicate its subscribers; moving is fine
	// because tokens capture slots, not events.
	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;
	Event(Event&&) = default;
	Event& operator=(Event&&) = default;

	[[nodiscard]] Subscription Subscribe(Callback callback)
	{
		auto slot = std::make_shared<Slot>(std::move(callback));
		slots_.push_back(slot);

		std::weak_ptr<Slot> weakSlot = slot;

		return Subscription([weakSlot]() {
			if (auto slot = weakSlot.lock())
				slot->alive = false; });
	}

	void Publish(Args... args)
	{
		// iterate over a snapshot. callbacks may subscribe, unsubscribe, or destroy
		// other subscribers while mid-dispatch
		const std::vector<std::shared_ptr<Slot>> snapshot = slots_;
		
		for (const auto& slot : snapshot)
			if (slot->alive) slot->callback(args...);

		slots_.erase(
			std::remove_if(slots_.begin(), slots_.end(),
				[](const std::shared_ptr<Slot>& s) { return !s->alive; }));
	}

private:
	struct Slot
	{
		explicit Slot(Callback c) : callback(std::move(c)) {}
		Callback callback;
		bool alive = true;
	};

	std::vector<std::shared_ptr<Slot>> slots_;
};

