#pragma once
#include "CriticalSection.hpp"
#include <vector>

typedef void(*basic_event_cb)(void *user_arg, void *event_arg);

class EventV0
{
	struct EventSub_t
	{
		basic_event_cb cb;
		void *user_arg;
	};

public:
	EventV0()
	{
	}

	~EventV0()
	{
		subscriptions.clear();
	}

	void subscribe(void *user_arg, basic_event_cb cb)
	{
		EventSub_t sub;
		sub.cb = cb;
		sub.user_arg = user_arg;
		subscriptions.push_back(sub);
	}

	void unsubscribe(void *user_arg, basic_event_cb cb)
	{
		for (int i = 0; i < (int)subscriptions.size(); ++i) 
		{
			EventSub_t &sub = subscriptions[i];
			if ((sub.cb == cb) && (sub.user_arg == user_arg)) 
			{
				subscriptions.erase(subscriptions.begin() + i);
				return;
			}
		}
	}

	void trigger(void *event_arg)
	{
		for (int i = 0; i < (int)subscriptions.size(); ++i) 
		{
			EventSub_t &sub = subscriptions[i];
			sub.cb(sub.user_arg, event_arg);
		}
	}

public:
	std::vector<EventSub_t> subscriptions;
};