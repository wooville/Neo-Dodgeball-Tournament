#pragma once

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class ProjectileLifecycleSystem : public System {
public:
	ProjectileLifecycleSystem() {
		RequireComponent<ProjectileComponent>();
	}

	void Update() {
		for (auto& entity : GetSystemEntities()) {
			auto& projectile = entity.GetComponent<ProjectileComponent>();

			if (entity.HasComponent<TransformComponent>() && entity.HasComponent<RigidBodyComponent>()) {
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
				float vX = rigidbody.velocityX;
				float vY = rigidbody.velocityY;
				float mag = sqrtf(vX * vX + vY * vY);

				// when a projectile slows down enough, turn it into a pickup
				if (mag < 0.33f || (glutGet(GLUT_ELAPSED_TIME) - projectile.startTime) > projectile.duration) {
					Entity newPickup = entity.registry->CreateEntity();
					newPickup.AddComponent<TransformComponent>(transform.x, transform.y);
					newPickup.AddComponent<SpriteComponent>(".\\Data\\Sprites\\green_square.bmp", 1, 1, 0);
					newPickup.AddComponent<BoxColliderComponent>(32, 32);
					newPickup.AddComponent<RigidBodyComponent>(vX, vY, 0.97);	// pickup maintains leftover velocity from projectile
					newPickup.Group("pickups");
					entity.Kill();
				}
			}

			/*if ((glutGet(GLUT_ELAPSED_TIME) - projectile.startTime) > projectile.duration) {
				entity.Kill();
			}*/
		}
	}
};