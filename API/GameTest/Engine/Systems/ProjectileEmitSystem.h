#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/PlayerActionEvent.h"
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
		eventBus->SubscribeToEvent<PlayerActionEvent>(this, &ProjectileEmitSystem::onPlayerAction);
	}

	void onPlayerAction(PlayerActionEvent& event) {
		for (auto entity : GetSystemEntities()) {
			//only process input of player
			if (entity.HasTag("player")) {
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				auto transform = entity.GetComponent<TransformComponent>();
				auto rigidbody = entity.GetComponent<RigidBodyComponent>();

				float projectilePositionX = transform.x;
				float projectilePositionY = transform.y;
				if (entity.HasComponent<SpriteComponent>()) {
					const auto sprite = entity.GetComponent<SpriteComponent>();
					projectilePositionX += (transform.scale * sprite.width / 2);
					projectilePositionY += (transform.scale * sprite.height / 2);
				}

				// adjust direction based on which way player is facing
				float projectileVelocityX = projectileEmitter.velocityX;
				float projectileVelocityY = projectileEmitter.velocityY;
				int directionX = 0;
				int directionY = 0;

				if (rigidbody.velocityX > 0) directionX = 1; 
				if (rigidbody.velocityX < 0) directionX = -1;
				if (rigidbody.velocityY > 0) directionY = 1;
				if (rigidbody.velocityY < 0) directionY = -1;

				projectileVelocityX = projectileEmitter.velocityX * directionX;
				projectileVelocityY = projectileEmitter.velocityY * directionY;


				Entity projectile = entity.registry->CreateEntity();
				projectile.Group("projectiles");
				projectile.AddComponent<TransformComponent>(projectilePositionX, projectilePositionY);
				projectile.AddComponent<RigidBodyComponent>(projectileVelocityX, projectileVelocityY);
				projectile.AddComponent<SpriteComponent>(".\\TestData\\projectile.bmp", 1, 1, 1);
				projectile.AddComponent<BoxColliderComponent>(32, 32);
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitDamage, projectileEmitter.projectileDuration);
			}
		}
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