#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/BallThrowEvent.h"
#include "../Events/CollisionEvent.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Events/ImportantEntityDiedEvent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ProjectileEmitterComponent.h"

#define AIM_TIME (1000.0f)
#define AIM_DISTANCE (300.0f)
#define CATCH_ACTIVE_TIME (3000.0f)
#define CATCH_CHARGE_TIME (10000.0f)
#define CHECK_PICKUP_INTERVAL (1000.0f)
#define DAMAGE_IMMUNITY_TIMER (1000.0f)

enum STATE {
	EVADE,
	PURSUE,
	PICKUP,
	CATCH,
	AIM
};

class EnemyBehaviour : public IScriptedBehaviour {
private:
	bool canCatch = false;
	float catchActiveTimer = 0.0;
	float catchChargeTimer = 0.0;
	float speed = 0.4f;
	float projectileSpeed = 0.5f;
	float checkPickupTimer = 0.0f;
	float dxPickup = 0.0f;
	float dyPickup = 0.0f;
	float immunityTimer = 0.0f;

public:
	bool patrolFlip = (rand() % 2); // decide if this enemy circles around the player clockwise or counterclockwise
	bool canThrow = false;
	bool canTakeDamage = true;
	float dxPlayer = 0.0f;
	float dyPlayer = 0.0f;
	float distanceToPlayer = 0.0f;
	float aimTimer = 0.0f;
	STATE currentState = STATE::EVADE;

	EnemyBehaviour() {
		//RequireComponent<PlayerAbilitiesComponent>();
		//RequireComponent<TransformComponent>();
		//RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		//eventBus->SubscribeToEvent<CollisionEvent>(this, &EnemyBehaviour::onCollision);
	}

	void Update(Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		auto& health = entity.GetComponent<HealthComponent>();
		auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
		auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

		if (health.health_val <= 0.0f) {
			eventBus->EmitEvent<ImportantEntityDiedEvent>(entity);
			entity.Kill();
		}

		if (!canTakeDamage) {
			immunityTimer += deltaTime;
			if (immunityTimer > DAMAGE_IMMUNITY_TIMER) {
				canTakeDamage = true;
				immunityTimer = 0.0f;
			}
		}

		if (!canCatch && currentState != STATE::CATCH) {
			catchChargeTimer += deltaTime;
			if (catchChargeTimer > CATCH_CHARGE_TIME) {
				canCatch = true;
				catchChargeTimer = 0.0f;
			}
		}
		
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& playerTransform = entity.registry->GetEntityByTag("player").GetComponent<TransformComponent>();

		// direction towards player
		dxPlayer = playerTransform.x - transform.x;
		dyPlayer = playerTransform.y - transform.y;
		distanceToPlayer = sqrtf(dxPlayer * dxPlayer + dyPlayer * dyPlayer);
		dxPlayer /= distanceToPlayer;
		dyPlayer /= distanceToPlayer;
		//projectileEmitter.velocityX = dx * projectileSpeed;
		//projectileEmitter.velocityY = dy * projectileSpeed;

		//bool pickupsAvailable = entity.registry->GetNumberOfEntitiesInGroup("pickups") > 0;
		std::vector<Entity> pickups = entity.registry->GetEntitiesByGroup("pickups");

		// find closest pickup if any exist
		float minDistance = INFINITY;
		for (Entity p : pickups) {
			auto& pTransform = p.GetComponent<TransformComponent>();
			float dx = pTransform.x - transform.x;
			float dy = pTransform.y - transform.y;
			float distanceToPickup = sqrtf(dx * dx + dy * dy);

			if (distanceToPickup < minDistance) {
				minDistance = distanceToPickup;
				dxPickup = dx / distanceToPickup;
				dyPickup = dy / distanceToPickup;
			}
		}

		if (canThrow) {
			if (distanceToPlayer < AIM_DISTANCE) {
				currentState = STATE::AIM;
			}
			else if (currentState != STATE::AIM) {
				currentState = STATE::PURSUE;
			}
		}
		else if (pickups.size()) {
			currentState = STATE::PICKUP;
		}
		else if (canCatch) {
			currentState = STATE::CATCH;
		}
		else {
			currentState = STATE::EVADE;
		}

		switch (currentState) {
		case EVADE:
			// move out to a safer distance
			if (distanceToPlayer < AIM_DISTANCE) {
				rigidbody.velocityX = -1 * dxPlayer * speed;
				rigidbody.velocityY = -1 * dyPlayer * speed;
			}
			else {	// circle the player at distance
				rigidbody.velocityX = (patrolFlip) ? -dyPlayer * speed : dyPlayer * speed;
				rigidbody.velocityY = (patrolFlip) ? dxPlayer * speed : -dxPlayer * speed;
			}

			// move away from edges
			if (transform.x > APP_VIRTUAL_WIDTH - 16.0f) {
				rigidbody.velocityX = -speed;
			}
			else if (transform.x <  16.0f) {
				rigidbody.velocityX = speed;
			}

			if (transform.y > APP_VIRTUAL_HEIGHT - 16.0f) {
				rigidbody.velocityY = -speed;
			}
			else if (transform.y < 16.0f) {
				rigidbody.velocityY = speed;
			}
			break;
		case PURSUE:
			if (distanceToPlayer > AIM_DISTANCE) {
				rigidbody.velocityX = dxPlayer * speed;
				rigidbody.velocityY = dyPlayer * speed;
			}
			break;
		case PICKUP:
			// head towards nearest pickup
			if (dxPickup != 0.0f && dyPickup != 0.0f) {
				rigidbody.velocityX = dxPickup * speed;
				rigidbody.velocityY = dyPickup * speed;
			}
			break;
		case CATCH:
			rigidbody.velocityX = 0.0f;
			rigidbody.velocityY = 0.0f;

			catchActiveTimer += deltaTime;
			if (catchActiveTimer > CATCH_ACTIVE_TIME) {
				EndCatch(false);
			}
			break;
		case AIM:
			rigidbody.velocityX = 0.0f;
			rigidbody.velocityY = 0.0f;

			// stored between state changes until it ticks over
			aimTimer += deltaTime*1.8;
			if (aimTimer > AIM_TIME - distanceToPlayer/4) {
				ReleaseChargingThrow(eventBus, entity);
			}
			break;
		default:
			break;
		}
		
	}

	void TakeDamage(HealthComponent& health, int damage) {
		if (canTakeDamage) {
			health.health_val -= damage;
			canTakeDamage = false;
			immunityTimer = 0.0f;
		}
	}

	void EndCatch(bool success) {
		canCatch = false;
		catchActiveTimer = 0.0;

		if (success) {
			canThrow = true;
		}
	}

	void ReleaseChargingThrow(std::unique_ptr<EventBus>& eventBus, Entity thrower) {
		eventBus->EmitEvent<BallThrowEvent>(thrower, dxPlayer, dyPlayer, aimTimer);
		aimTimer = 0.0f;
		canThrow = false;
	}

	void Pickup(Entity pickup) {
		if (!canThrow) {
			canThrow = true;
			pickup.Kill();
		}
	}
};
