#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptedBehaviourComponent.h"

class ScriptedBehaviourSystem: public System {
public:
	ScriptedBehaviourSystem() {
		RequireComponent<ScriptedBehaviourComponent>();
	}

	// Update each script's event subscriptions, defined individually
	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		for (auto entity : GetSystemEntities()) {
			auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();

			scriptedBehaviour.script->SubscribeToEvents(eventBus);
		}
	}

	void Update(std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		for (auto entity : GetSystemEntities()) {
			auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();

			scriptedBehaviour.script->Update(entity, eventBus, deltaTime);
		}
	}
};