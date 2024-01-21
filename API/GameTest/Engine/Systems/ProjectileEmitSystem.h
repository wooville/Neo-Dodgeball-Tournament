#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/BallThrowEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"

class ProjectileEmitSystem : public System {
public:
	ProjectileEmitSystem() {
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<BallThrowEvent>(this, &ProjectileEmitSystem::onBallThrow);
	}

	void onBallThrow(BallThrowEvent& event) {
		Entity entity = event.thrower;

		//if (thrower.HasTag("player")) {
		auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
		auto transform = entity.GetComponent<TransformComponent>();

		float projectilePositionX = transform.x;
		float projectilePositionY = transform.y;
		//if (entity.HasComponent<SpriteComponent>()) {
		//	const auto sprite = entity.GetComponent<SpriteComponent>();
		//	projectilePositionX += (transform.scale * sprite.width / 2);
		//	projectilePositionY += (transform.scale * sprite.height / 2);
		//}

		// set initial velocity based on charge time
		projectileEmitter.velocityX = event.throwCharge / 1000.0;
		projectileEmitter.velocityY = event.throwCharge / 1000.0;

		// adjust direction based on where thrower was aiming
		float projectileVelocityX = projectileEmitter.velocityX * event.throwDirectionX;
		float projectileVelocityY = projectileEmitter.velocityY * event.throwDirectionY;

		Entity projectile = entity.registry->CreateEntity();
		projectile.Group("projectiles");
		projectile.AddComponent<TransformComponent>(projectilePositionX, projectilePositionY);
		projectile.AddComponent<RigidBodyComponent>(projectileVelocityX, projectileVelocityY, 0.97f);
		projectile.AddComponent<SpriteComponent>(".\\TestData\\projectile.bmp", 1, 1, 1);
		projectile.AddComponent<BoxColliderComponent>(32, 32);
		projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitDamage, projectileEmitter.projectileDuration);
		
	}

	void Update(std::unique_ptr<Registry>& registry) {
		for (auto entity : GetSystemEntities()) {
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			auto transform = entity.GetComponent<TransformComponent>();

			if (projectileEmitter.repeatFreq == 0) {
				continue;
			}

			// check re-emit
			if (glutGet(GLUT_ELAPSED_TIME) - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFreq) {
				float projectilePositionX = transform.x;
				float projectilePositionY = transform.y;
				if (entity.HasComponent<SpriteComponent>()) {
					const auto sprite = entity.GetComponent<SpriteComponent>();
					projectilePositionX += (transform.scale * sprite.width / 2);
					projectilePositionY += (transform.scale * sprite.height / 2);
				}

				Entity projectile = registry->CreateEntity();
				projectile.Group("projectiles");
				projectile.AddComponent<TransformComponent>(projectilePositionX, projectilePositionY);
				projectile.AddComponent<RigidBodyComponent>(projectileEmitter.velocityX, projectileEmitter.velocityY);
				projectile.AddComponent<SpriteComponent>(".\\TestData\\projectile.bmp", 1, 1, 1);
				projectile.AddComponent<BoxColliderComponent>(32, 32);
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitDamage, projectileEmitter.projectileDuration);

				// update last emission time
				projectileEmitter.lastEmissionTime = glutGet(GLUT_ELAPSED_TIME);
			}
		}
	}
};