#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include <iostream>

class CollisionSystem : public System {
public:
	CollisionSystem() {
		RequireComponent<BoxColliderComponent>();
		RequireComponent<TransformComponent>();
	}

	bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
		return (
			aX < bX + bW &&
			aX + aW > bX &&
			aY < bY + bH &&
			aY + aH > bY
			);
	}

	void Update(std::unique_ptr<EventBus>& eventBus) {
		auto entities = GetSystemEntities();

		// Loop relevant entities
		for (auto i = entities.begin(); i != entities.end(); i++) {
			Entity a = *i;
			auto aTransform = a.GetComponent<TransformComponent>();
			auto aCollider = a.GetComponent<BoxColliderComponent>();

			// Loop remainder of list (right of i)
			for (auto j = i; j != entities.end(); j++) {
				Entity b = *j;

				if (a == b) {
					continue;
				}

				auto bTransform = b.GetComponent<TransformComponent>();
				auto bCollider = b.GetComponent<BoxColliderComponent>();

				bool collisionAB = CheckAABBCollision(
					aTransform.x + aCollider.offsetX,
					aTransform.y + aCollider.offsetY,
					aCollider.width,
					aCollider.height,
					bTransform.x + bCollider.offsetX,
					bTransform.y + bCollider.offsetY,
					bCollider.width,
					bCollider.height
				);

				if (collisionAB) {
					//Logger::Log("Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
					
					eventBus->EmitEvent<CollisionEvent>(a, b);
				}
			}
		}
	}
};