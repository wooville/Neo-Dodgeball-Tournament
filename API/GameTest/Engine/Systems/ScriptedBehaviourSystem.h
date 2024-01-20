#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptedBehaviourComponent.h"

class ScriptedBehaviourSystem: public System {
public:
	ScriptedBehaviourSystem() {
		RequireComponent<ScriptedBehaviourComponent>();
	}

	// Update each script's event subscriptions
	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		for (auto entity : GetSystemEntities()) {
			auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();

			if (entity.HasTag("player")) {
				scriptedBehaviour.script = std::static_pointer_cast<PlayerBehaviour>(scriptedBehaviour.script);
			}

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