#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/PlayerActionEvent.h"
#include "../Events/CollisionEvent.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ProjectileEmitterComponent.h"

class EnemyBehaviour : public IScriptedBehaviour {
private:
	bool canAim = false;
	bool canThrow = false;
	bool canCatch = false;
	bool defeated = false;
	float catchActiveTimer = 0.0;
	float catchCooldownTimer = 0.0;
	float speed = 0.5f;
	float projectileSpeed = 0.5f;

public:
	bool isCatching = false;

	EnemyBehaviour() {
		//RequireComponent<PlayerAbilitiesComponent>();
		//RequireComponent<TransformComponent>();
		//RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		//eventBus->SubscribeToEvent<CollisionEvent>(this, &EnemyBehaviour::onCollision);
	}

	void Update(std::unique_ptr<Registry>& registry, Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		if (defeated) {
			defeated = false;
			eventBus->EmitEvent<ScoreChangeEvent>(10);
			entity.Kill();
		}

		auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
		auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
		
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& playerTransform = registry->GetEntityByTag("player").GetComponent<TransformComponent>();

		// aim at player position
		float dx = playerTransform.x - transform.x;
		float dy = playerTransform.y - transform.y;
		float dist = sqrtf(dx*dx+dy*dy);
		dx /= dist;
		dy /= dist;
		projectileEmitter.velocityX = dx*projectileSpeed;
		projectileEmitter.velocityY = dy*projectileSpeed;
	}

	void Defeat() {
		defeated = true;
	}

	//void onCollision(CollisionEvent& event) {
	//	Entity a = event.b;
	//	Entity b = event.b;
	//	if (a.BelongsToGroup("pickups")) {
	//		onPickup(a);
	//	}
	//	if (b.BelongsToGroup("pickups")) {
	//		onPickup(b);
	//	}
	//}
};
