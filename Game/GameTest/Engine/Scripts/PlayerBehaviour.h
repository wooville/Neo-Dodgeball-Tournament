#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/BallThrowEvent.h"
#include "../Events/CollisionEvent.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Events/ImportantEntityDiedEvent.h"
#include "../Events/ResetGameEvent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TextComponent.h"

// timers in ms
#define SPAWN_COORD_X (APP_VIRTUAL_WIDTH/2)
#define SPAWN_COORD_Y (APP_VIRTUAL_HEIGHT/2)
#define HP_COORD_X (50.0f)
#define HP_COORD_Y (APP_VIRTUAL_HEIGHT - 100.0f)
#define CATCH_COORD_X (HP_COORD_X)
#define CATCH_COORD_Y (HP_COORD_Y - 50.0f)
#define CONTROLS_ANCHOR_COORD_X (50.0f)
#define CONTROLS_ANCHOR_COORD_Y (50.0f)
#define CATCH_ACTIVE_TIME (250.0f)
#define CATCH_COOLDOWN_TIME (2000.0f)
#define SCORE_CHANGE_CATCH (1)
#define MAX_THROW_CHARGE (2000.0f)
#define DAMAGE_IMMUNITY_TIMER (1000.0f)

class PlayerBehaviour : public IScriptedBehaviour {
private:
	bool canCatch = false;
	bool caughtBall = false;
	float catchActiveTimer = 0.0;
	float catchCooldownTimer = 0.0;
	float speed = 0.4f;
	float immunityTimer = 0.0f;

public:
	bool canThrow = false;
	bool isAiming = false;
	bool isCatching = false;
	bool isChargingThrow = false;
	bool canTakeDamage = true;
	bool isDead = false;
	float throwCharge = 0.0f;
	float throwDirectionX = 0.0f;
	float throwDirectionY = 0.0f;

	PlayerBehaviour() {}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<ResetGameEvent>(this, &PlayerBehaviour::onResetGameEvent);
	}

	void Update(Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		auto& health = entity.GetComponent<HealthComponent>();
		auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
		auto& controller = App::GetController();

		if (health.health_val <= 0.0f) {
			eventBus->EmitEvent<ImportantEntityDiedEvent>(entity);
			isDead = true;
		}

		if (!canTakeDamage) {
			immunityTimer += deltaTime;
			if (immunityTimer > DAMAGE_IMMUNITY_TIMER) {
				canTakeDamage = true;
				immunityTimer = 0.0f;
			}
		}

		if (isDead) {
			if (controller.CheckButton(XINPUT_GAMEPAD_A, true))
			{
				ResetPlayer(entity);
				eventBus->EmitEvent<ResetGameEvent>(entity);
			}
		}
		else {
			if (isAiming) {
				// normalize left thumbstick for aim direction
				throwDirectionX = controller.GetLeftThumbStickX();
				throwDirectionY = controller.GetLeftThumbStickY();
				float mag = sqrtf(throwDirectionX * throwDirectionX + throwDirectionY * throwDirectionY);
				throwDirectionX /= mag;
				throwDirectionY /= mag;

				rigidbody.velocityX = 0.0f;
				rigidbody.velocityY = 0.0f;
			}
			else {
				if (controller.GetLeftThumbStickX() > 0.5f || controller.CheckButton(XINPUT_GAMEPAD_DPAD_RIGHT, false))
				{
					rigidbody.velocityX = speed;
					//testSprite->SetAnimation(ANIM_RIGHT);
				}
				else if (controller.GetLeftThumbStickX() < -0.5f || controller.CheckButton(XINPUT_GAMEPAD_DPAD_LEFT, false))
				{
					rigidbody.velocityX = -speed;
					//testSprite->SetAnimation(ANIM_LEFT);
				}
				else
				{
					rigidbody.velocityX = 0.0f;
				}

				if (controller.GetLeftThumbStickY() > 0.5f || controller.CheckButton(XINPUT_GAMEPAD_DPAD_UP, false))
				{
					rigidbody.velocityY = speed;
					//testSprite->SetAnimation(ANIM_FORWARDS);
				}
				else if (controller.GetLeftThumbStickY() < -0.5f || controller.CheckButton(XINPUT_GAMEPAD_DPAD_DOWN, false))
				{
					rigidbody.velocityY = -speed;
					//testSprite->SetAnimation(ANIM_BACKWARDS);
				}
				else
				{
					rigidbody.velocityY = 0.0f;
				}
			}

			// start aiming if can throw and holding LT, stop aiming with LT release
			if (canThrow && !isAiming && controller.GetLeftTrigger() > 0.2f)
			{
				// can't catch while aiming
				StartAiming();
			}
			else if (isAiming && controller.GetLeftTrigger() < 0.2f) {
				// cancel any charging throws
				StopAiming();
			}

			// start charging throw if aiming and holding RT
			if (isAiming && controller.GetRightTrigger() > 0.2f) {
				StartChargingThrow();
			}

			// release RT while charging to throw
			if (isAiming && isChargingThrow && controller.GetRightTrigger() < 0.2f) {
				ReleaseChargingThrow(eventBus, entity);
			}

			if (isChargingThrow) {
				throwCharge += deltaTime * 1.8f;
				if (throwCharge > MAX_THROW_CHARGE) {
					throwCharge = MAX_THROW_CHARGE;
				}
			}

			if (canCatch && controller.CheckButton(XINPUT_GAMEPAD_A, true))
			{
				StartCatch();
			}

			if (isCatching) {
				catchActiveTimer += deltaTime;
				if (catchActiveTimer > CATCH_ACTIVE_TIME) {
					EndCatch(false);
				}
			}

			if (caughtBall) {
				eventBus->EmitEvent<ScoreChangeEvent>(SCORE_CHANGE_CATCH);
				caughtBall = false;
			}

			if (!canCatch) {
				catchCooldownTimer += deltaTime;
				if (catchCooldownTimer > CATCH_COOLDOWN_TIME) {
					canCatch = true;
				}
			}

			if (controller.CheckButton(XINPUT_GAMEPAD_B, true))
			{
				App::PlaySound(".\\Data\\Sound\\Test.wav");
			}

			auto& textComponent = entity.GetComponent<TextComponent>();
			std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;
			std::pair<float, float> coords = std::make_pair(HP_COORD_X, HP_COORD_Y);
			textToRender.emplace_back(coords, "HP " + std::to_string(health.health_val));

			coords = std::make_pair(CATCH_COORD_X, CATCH_COORD_Y);
			if (canCatch) {
				textToRender.emplace_back(coords, "Can Catch");
			}
			else {
				textToRender.emplace_back(coords, "");
			}

			coords = std::make_pair(CONTROLS_ANCHOR_COORD_X, CONTROLS_ANCHOR_COORD_Y);
			if (!isAiming) {
				textToRender.emplace_back(coords, "LS - MOVE");
			}
			else {
				textToRender.emplace_back(coords, "LS - AIM");
			}

			coords = std::make_pair(CONTROLS_ANCHOR_COORD_X, CONTROLS_ANCHOR_COORD_Y + 50.0f);
			if (!isAiming) {
				textToRender.emplace_back(coords, "LT - AIM MODE");
			}
			else {
				textToRender.emplace_back(coords, "LT RELEASE - CANCEL");
			}

			coords = std::make_pair(CONTROLS_ANCHOR_COORD_X, CONTROLS_ANCHOR_COORD_Y + 100.0f);
			if (!isAiming) {
				textToRender.emplace_back(coords, "A - CATCH");
			}
			else if (!isChargingThrow) {
				textToRender.emplace_back(coords, "RT - CHARGE THROW");
			}
			else {
				textToRender.emplace_back(coords, "RT RELEASE - THROW");
			}

			textComponent.textToRender = textToRender;
		}
	}

	void StartAiming() {
		isAiming = true;
		isCatching = false;
		canCatch = false;
	}

	void StopAiming() {
		isAiming = false;
		canCatch = true;
		isChargingThrow = false;
		throwCharge = 0.0f;
	}

	void StartChargingThrow() {
		isChargingThrow = true;
	}

	void ReleaseChargingThrow(std::unique_ptr<EventBus>& eventBus, Entity thrower) {
		isChargingThrow = false;
		canThrow = false;
		eventBus->EmitEvent<BallThrowEvent>(thrower, throwDirectionX, throwDirectionY, throwCharge);
		StopAiming();
	}

	void StartCatch() {
		canCatch = false;
		isCatching = true;
	}

	void EndCatch(bool success) {
		catchActiveTimer = 0.0;
		isCatching = false;

		if (success) {
			caughtBall = true;
			canCatch = true;
			canThrow = true;
			catchCooldownTimer = 0.0;
		}
	}

	void TakeDamage(HealthComponent& health, int damage) {
		if (canTakeDamage) {
			health.health_val -= damage;
			canTakeDamage = false;
			immunityTimer = 0.0f;
		}
	}

	void Pickup(Entity pickup) {
		if (!canThrow) {
			canThrow = true;
			pickup.Kill();
		}
	}

	void ResetPlayer(Entity entity) {
		auto& health = entity.GetComponent<HealthComponent>();
		auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
		auto& transform = entity.GetComponent<TransformComponent>();

		transform.x = SPAWN_COORD_X;
		transform.y = SPAWN_COORD_Y;

		health.health_val = 5;

		rigidbody.velocityX = 0.0f;
		rigidbody.velocityY = 0.0f;

		canThrow = false;
		isDead = false;
	}

	// cheeky last minute workaround to reuse resetgameevent
	void onResetGameEvent(ResetGameEvent& event) {
		if (event.a.HasTag("manager")) {
			isDead = true;	
		}
	}
};
