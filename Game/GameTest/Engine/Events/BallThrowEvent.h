#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class BallThrowEvent : public Event {
public:
	Entity thrower;
	float throwDirectionX;
	float throwDirectionY;
	float throwCharge;

	BallThrowEvent(Entity thrower, float throwDirectionX, float throwDirectionY, float throwCharge): thrower(thrower), throwDirectionX(throwDirectionX), throwDirectionY(throwDirectionY), throwCharge(throwCharge) {}
};