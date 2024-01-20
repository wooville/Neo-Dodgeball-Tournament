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

		//if (a.BelongsToGroup("enemies") && b.HasTag("player")) {
		//	onEnemyPlayerCollision(a, b);
		//}
		//if (b.BelongsToGroup("enemies") && a.HasTag("player")) {
		//	onEnemyPlayerCollision(b, a);
		//}
		
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



	//void onEnemyPlayerCollision(Entity enemy, Entity player) {
	//	//auto projectileComponent = enemy.GetComponent<ProjectileComponent>();

	//	
	//	auto& health = enemy.GetComponent<HealthComponent>();

	//	health.health_val -= 10;

	//	if (health.health_val <= 0) {
	//		enemy.Kill();
	//	}
	//	// TODO timer
	//}

	void onProjectileEnemyCollision(Entity projectile, Entity enemy) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		// update health, check health for kill condition, kill projectile entity
		if (projectileComponent.isFriendly) {
			auto& health = enemy.GetComponent<HealthComponent>();

			health.health_val -= projectileComponent.hitDamage;

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
				projectile.Kill();
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

	void Update() {

	}
};