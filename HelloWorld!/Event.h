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

	Subscription(Subscription&& other) noexcept : kill(std::move(other.kill)) {}
	Subscription& operator=(Subscription&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			kill = std::move(other.kill);
		}

		return *this;
	}

	void Reset()
	{
		if (kill) 
		{ 
			kill();  kill = nullptr; 
		}
	}

private:
	template <typename... Args> friend class Event;
	explicit Subscription(std::function<void()>&& kill) : kill(std::move(kill)) {}

	std::function<void()> kill;
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
		slots.push_back(slot);

		std::weak_ptr<Slot> weakSlot = slot;

		return Subscription([weakSlot]() {
			if (auto slot = weakSlot.lock())
				slot->alive = false; });
	}

	void Publish(Args... args)
	{
		// iterate over a snapshot. callbacks may subscribe, unsubscribe, or destroy
		// other subscribers while mid-dispatch
		const std::vector<std::shared_ptr<Slot>> snapshot = slots;
		
		for (const auto& slot : snapshot)
			if (slot->alive) slot->callback(args...);

		slots.erase(
			std::remove_if(slots.begin(), slots.end(),
				[](const std::shared_ptr<Slot>& s) { return !s->alive; }),
			slots.end());
	}

private:
	struct Slot
	{
		explicit Slot(Callback c) : callback(std::move(c)) {}
		Callback callback;
		bool alive = true;
	};

	std::vector<std::shared_ptr<Slot>> slots;
};

