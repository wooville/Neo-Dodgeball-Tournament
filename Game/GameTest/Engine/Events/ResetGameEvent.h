#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class ResetGameEvent : public Event {
public:
	Entity a;

	ResetGameEvent(Entity a) : a(a) {}
};