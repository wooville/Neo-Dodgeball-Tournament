#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class ImportantEntityDiedEvent : public Event {
public:
	Entity entity;

	ImportantEntityDiedEvent(Entity entity): entity(entity) {}
};