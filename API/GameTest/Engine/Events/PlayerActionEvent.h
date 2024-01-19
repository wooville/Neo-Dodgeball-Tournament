#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class PlayerActionEvent : public Event {
public:
	PlayerActionEvent() {}
};