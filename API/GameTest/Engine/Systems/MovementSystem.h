#pragma once

#include "../App/AppSettings.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class MovementSystem : public System {
public:
	MovementSystem() {
		 RequireComponent<TransformComponent>();
		 RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;
		//Logger::Log("Movement system received event collision between entities " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));

		if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
			onEnemyObstacleCollision(a, b);
		}
		if (b.BelongsToGroup("enemies") && a.BelongsToGroup("obstacles")) {
			onEnemyObstacleCollision(b, a);
		}
	}

	void onEnemyObstacleCollision(Entity enemy, Entity obstacle) {
		if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
			auto& rigidbody = enemy.GetComponent<RigidBodyComponent>();
			auto& sprite = enemy.GetComponent<SpriteComponent>();
			Logger::Log("Movement system received event collision between entities " + std::to_string(enemy.GetId()) + " and " + std::to_string(obstacle.GetId()));

			if (rigidbody.velocityX != 0) {
				rigidbody.velocityX *= -1;
				//sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}

			if (rigidbody.velocityY != 0) {
				rigidbody.velocityY *= -1;
				//sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}
	}

	void Update(double deltaTime) {
		for (auto entity : GetSystemEntities()) {
			//update position based on velocity
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

			transform.x += rigidbody.velocityX*deltaTime;
			transform.y += rigidbody.velocityY*deltaTime;

			if (entity.HasTag("player")) {
				int paddingTop = 8;
				int paddingRight = 8;
				int paddingDown = 8;
				int paddingLeft = 8;
				transform.x = transform.x < paddingLeft ? paddingLeft : transform.x;
				transform.x = transform.x > APP_VIRTUAL_WIDTH - paddingRight ? APP_VIRTUAL_WIDTH - paddingRight : transform.x;
				transform.y = transform.y < paddingLeft ? paddingLeft : transform.y;
				transform.y = transform.y > APP_VIRTUAL_HEIGHT - paddingRight ? APP_VIRTUAL_HEIGHT - paddingRight : transform.y;
			}

			bool isEntityOutsideMap = (
				transform.x < 0 ||
				transform.x > APP_VIRTUAL_WIDTH ||
				transform.y < 0 ||
				transform.y > APP_VIRTUAL_HEIGHT
			);

			if (isEntityOutsideMap && !entity.HasTag("player")) {
				entity.Kill();
			}
		}
	}
};