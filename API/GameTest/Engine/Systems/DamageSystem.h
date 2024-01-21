#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ScriptedBehaviourComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Scripts/PlayerBehaviour.h"
#include "../Scripts/EnemyBehaviour.h"

class DamageSystem : public System {
public:
	DamageSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;

		if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("walls")) {
			onProjectileWallCollision(a, b);
		}
		if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("walls")) {
			onProjectileWallCollision(b, a);
		}

		if (a.BelongsToGroup("pickups")) {
			onPickupCollision(a, b);
		}
		if (b.BelongsToGroup("pickups")) {
			onPickupCollision(b, a);
		}
		
		if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
			onProjectilePlayerCollision(a, b);
		}
		if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
			onProjectilePlayerCollision(b, a);
		}

		if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
			onProjectileEnemyCollision(a, b);
		}
		if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
			onProjectileEnemyCollision(b, a);
		}
	}

	void onProjectileWallCollision(Entity projectile, Entity wall) {
		auto& transform = projectile.GetComponent<TransformComponent>();
		auto& rigidbody = projectile.GetComponent<RigidBodyComponent>();

		// bounce off of wall
		if (wall.BelongsToGroup("reverseX")) {
			rigidbody.velocityX *= -1;
		}
		else if (wall.BelongsToGroup("reverseY")) {
			rigidbody.velocityY *= -1;
		}

		// spawn pickup to replace projectile
		/*Entity newPickup = projectile.registry->CreateEntity();
		newPickup.AddComponent<TransformComponent>(transform.x, transform.y);
		newPickup.AddComponent<SpriteComponent>(".\\TestData\\green_square.bmp", 1, 1, 0);
		newPickup.AddComponent<BoxColliderComponent>(32, 32);
		newPickup.Group("pickups");
		projectile.Kill();*/
		
	}

	void onProjectileEnemyCollision(Entity projectile, Entity enemy) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		// update health, check health for kill condition, kill projectile entity
		if (projectileComponent.isFriendly) {
			auto& health = enemy.GetComponent<HealthComponent>();
			auto& scriptedBehaviour = enemy.GetComponent<ScriptedBehaviourComponent>();
			auto& enemyBehaviour = std::static_pointer_cast<EnemyBehaviour>(scriptedBehaviour.script);

			if (enemyBehaviour->currentState == STATE::CATCH) {
				App::PlaySound(".\\TestData\\Test.wav");
				enemyBehaviour->EndCatch(true);
			}
			else
			{
				health.health_val -= projectileComponent.hitDamage;
			}

			if (health.health_val <= 0) {
				auto& scriptedBehaviour = enemy.GetComponent<ScriptedBehaviourComponent>();
				auto& enemyBehaviour = std::static_pointer_cast<EnemyBehaviour>(scriptedBehaviour.script);

				enemyBehaviour->Defeat();

				//enemy.Kill();
			}

			projectile.Kill();
		}
	}

	void onProjectilePlayerCollision(Entity projectile, Entity player) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		// update health, check health for kill condition, kill projectile entity
		if (!projectileComponent.isFriendly) {
			auto& health = player.GetComponent<HealthComponent>();
			auto& scriptedBehaviour = player.GetComponent<ScriptedBehaviourComponent>();
			auto& playerBehaviour = std::static_pointer_cast<PlayerBehaviour>(scriptedBehaviour.script);

			if (playerBehaviour->isCatching) {
				App::PlaySound(".\\TestData\\Test.wav");
				playerBehaviour->EndCatch(true);
			}
			else
			{
				health.health_val -= projectileComponent.hitDamage;
			}

			if (health.health_val <= 0) {
				player.Kill();
			}

			projectile.Kill();
		}
	}

	void onPickupCollision(Entity pickup, Entity entity) {
		if (entity.HasTag("player") && entity.HasComponent<ScriptedBehaviourComponent>()) {
			auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();
			auto& playerBehaviour = std::static_pointer_cast<PlayerBehaviour>(scriptedBehaviour.script);
			playerBehaviour->Pickup(pickup);
		}
		else if (entity.BelongsToGroup("enemies") && entity.HasComponent<ScriptedBehaviourComponent>()) {
			auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();
			auto& enemyBehaviour = std::static_pointer_cast<EnemyBehaviour>(scriptedBehaviour.script);
			enemyBehaviour->Pickup(pickup);
		}
	}

	void Update() {

	}
};