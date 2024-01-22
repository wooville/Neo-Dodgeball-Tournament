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

class CollisionListenerSystem : public System {
public:
	CollisionListenerSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &CollisionListenerSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;

		if (a.BelongsToGroup("enemies") && b.BelongsToGroup("enemies")) {
			onEnemyEnemyCollision(a, b);
		}

		if (a.BelongsToGroup("projectiles") && a.BelongsToGroup("projectiles")) {
			onProjectileProjectileCollision(a,b);
		}

		if ((a.BelongsToGroup("projectiles") || a.BelongsToGroup("pickups")) && b.BelongsToGroup("walls")) {
			onProjectileWallCollision(a, b);
		}
		if ((b.BelongsToGroup("projectiles") || b.BelongsToGroup("pickups")) && a.BelongsToGroup("walls")) {
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

		auto& pTransform = projectile.GetComponent<TransformComponent>();
		auto& eTransform = wall.GetComponent<TransformComponent>();

		auto& pBoxCollider = projectile.GetComponent<BoxColliderComponent>();
		auto& eBoxCollider = wall.GetComponent<BoxColliderComponent>();

		// bounce off of wall
		if (wall.BelongsToGroup("reverseX")) {
			rigidbody.velocityX *= -1;
		}
		else if (wall.BelongsToGroup("reverseY")) {
			rigidbody.velocityY *= -1;
		}
	}

	void onProjectileEnemyCollision(Entity projectile, Entity enemy) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
		auto& rigidbody = projectile.GetComponent<RigidBodyComponent>();

		// update health, check health for kill condition, kill projectile entity
		if (projectileComponent.isFriendly) {
			auto& health = enemy.GetComponent<HealthComponent>();
			auto& scriptedBehaviour = enemy.GetComponent<ScriptedBehaviourComponent>();
			auto& enemyBehaviour = std::static_pointer_cast<EnemyBehaviour>(scriptedBehaviour.script);

			if (enemyBehaviour->currentState == STATE::CATCH) {
				App::PlaySound(".\\Data\\Sound\\Test.wav");
				enemyBehaviour->EndCatch(true);
				projectile.Kill();
			}
			else if (enemyBehaviour->canTakeDamage)
			{
				enemyBehaviour->TakeDamage(health, projectileComponent.hitDamage);
				ReflectVelocity(projectile, enemy);
			}
		}
	}

	void onProjectilePlayerCollision(Entity projectile, Entity player) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
		auto& rigidbody = projectile.GetComponent<RigidBodyComponent>();

		// update health, check health for kill condition, kill projectile entity
		if (!projectileComponent.isFriendly) {
			auto& health = player.GetComponent<HealthComponent>();
			auto& scriptedBehaviour = player.GetComponent<ScriptedBehaviourComponent>();
			auto& playerBehaviour = std::static_pointer_cast<PlayerBehaviour>(scriptedBehaviour.script);

			if (playerBehaviour->isCatching) {
				// drop ball if player is already holding one
				if (playerBehaviour->canThrow) {
					auto& transform = player.GetComponent<TransformComponent>();
					Entity newPickup = player.registry->CreateEntity();
					newPickup.AddComponent<TransformComponent>(transform.x, transform.y);
					newPickup.AddComponent<SpriteComponent>(".\\Data\\Sprites\\green_square.bmp", 1, 1, 0);
					newPickup.AddComponent<BoxColliderComponent>(32, 32);
					newPickup.Group("pickups");
				}

				App::PlaySound(".\\Data\\Sound\\Test.wav");
				playerBehaviour->EndCatch(true);
				projectile.Kill();
			}
			else if (playerBehaviour->canTakeDamage)
			{
				//health.health_val -= projectileComponent.hitDamage;
				playerBehaviour->TakeDamage(health, projectileComponent.hitDamage);
				ReflectVelocity(projectile, player);
			}
		}
	}

	// bounce off of each other
	// projectile with the higher velocity converts the other projectile to its side
	void onProjectileProjectileCollision(Entity p1, Entity p2) {
		// I lost many hours of my life to this small block
		/*auto pc1 = p1.GetComponent<ProjectileComponent>();
		auto pc2 = p2.GetComponent<ProjectileComponent>();

		auto& rb1 = p1.GetComponent<RigidBodyComponent>();
		auto& rb2 = p2.GetComponent<RigidBodyComponent>();

		float mag1 = sqrtf(rb1.velocityX * rb1.velocityX + rb1.velocityY * rb1.velocityY);
		float mag2 = sqrtf(rb2.velocityX * rb2.velocityX + rb2.velocityY * rb2.velocityY);

		if (mag1 > mag2) {
			pc2.isFriendly = pc1.isFriendly;
		}
		else {
			pc1.isFriendly = pc2.isFriendly;
		}*/

		ReflectVelocity(p1, p2);
		//ReflectVelocity(p2, p1);
	}

	// reverse one enemy's patrol direction
	void onEnemyEnemyCollision(Entity e1, Entity e2) {
		auto& scriptedBehaviour = e1.GetComponent<ScriptedBehaviourComponent>();
		auto& enemyBehaviour = std::static_pointer_cast<EnemyBehaviour>(scriptedBehaviour.script);

		enemyBehaviour->patrolFlip = !enemyBehaviour->patrolFlip;
	}

	void ReflectVelocity(Entity projectile, Entity hit) {
		auto& rigidbody = projectile.GetComponent<RigidBodyComponent>();

		auto& pTransform = projectile.GetComponent<TransformComponent>();
		auto& hTransform = hit.GetComponent<TransformComponent>();

		auto& pBoxCollider = projectile.GetComponent<BoxColliderComponent>();
		auto& hBoxCollider = hit.GetComponent<BoxColliderComponent>();

		if ((pTransform.x + pBoxCollider.offsetX + pBoxCollider.width < hTransform.x + hBoxCollider.offsetX + hBoxCollider.width / 2.0f) ||
			(pTransform.x + pBoxCollider.offsetX > hTransform.x + hBoxCollider.offsetX + hBoxCollider.width / 2.0f)) {
			rigidbody.velocityX *= -1;
		}

		if ((pTransform.y + pBoxCollider.offsetY + pBoxCollider.height < hTransform.y + hBoxCollider.offsetY + hBoxCollider.height / 2.0f) ||
			(pTransform.y + pBoxCollider.offsetY > hTransform.y + hBoxCollider.offsetY + hBoxCollider.height / 2.0f)) {
			rigidbody.velocityY *= -1;
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