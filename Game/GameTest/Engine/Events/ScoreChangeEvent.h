#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class ScoreChangeEvent : public Event {
public:
	int deltaScore;

	ScoreChangeEvent(int deltaScore): deltaScore(deltaScore) {}
};