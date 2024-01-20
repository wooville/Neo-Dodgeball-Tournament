#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/PlayerActionEvent.h"
#include "../Events/CollisionEvent.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Components/PlayerAbilitiesComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TextComponent.h"

// timers in ms
#define HP_COORD_X (100.0f)
#define HP_COORD_Y (APP_VIRTUAL_HEIGHT - 100.0f)
#define CATCH_ACTIVE_TIME (250.0f)
#define CATCH_COOLDOWN_TIME (2000.0f)
#define SCORE_CHANGE_CATCH (1)

class PlayerBehaviour : public IScriptedBehaviour {
private:
	bool canAim = false;
	bool canThrow = false;
	bool canCatch = false;
	bool caughtBall = false;
	float catchActiveTimer = 0.0;
	float catchCooldownTimer = 0.0;
	float speed = 0.5f;

public:
	bool isCatching = false;

	PlayerBehaviour() {
		//RequireComponent<PlayerAbilitiesComponent>();
		//RequireComponent<TransformComponent>();
		//RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &PlayerBehaviour::onCollision);
	}

	void Update(std::unique_ptr<Registry>& registry, Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
		auto& controller = App::GetController();


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

		if (canThrow && controller.CheckButton(XINPUT_GAMEPAD_X, true))
		{
			eventBus->EmitEvent<PlayerActionEvent>();
			canThrow = false;
		}

		if (canCatch && controller.CheckButton(XINPUT_GAMEPAD_A, true))
		{
			startCatch();
		}

		if (isCatching) {
			catchActiveTimer += deltaTime;
			if (catchActiveTimer > CATCH_ACTIVE_TIME) {
				endCatch(false);
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
			App::PlaySound(".\\TestData\\Test.wav");
		}

		auto& textComponent = entity.GetComponent<TextComponent>();
		auto& healthComponent = entity.GetComponent<HealthComponent>();
		std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;
		std::pair<float, float> coords = std::make_pair(HP_COORD_X, HP_COORD_Y);
		textToRender.emplace_back(coords, std::to_string(healthComponent.health_val));
		textComponent.textToRender = textToRender;
	}

	void startCatch() {
		canCatch = false;
		isCatching = true;
	}

	void endCatch(bool success) {
		catchActiveTimer = 0.0;
		isCatching = false;

		if (success) {
			caughtBall = true;
			canCatch = true;
			canThrow = true;
			catchCooldownTimer = 0.0;
		}
	}

	void onCollision(CollisionEvent& event) {
		Entity a = event.b;
		Entity b = event.b;
		if (a.BelongsToGroup("pickups")) {
			onPickup(a);
		}
		if (b.BelongsToGroup("pickups")) {
			onPickup(b);
		}
	}

	void onPickup(Entity pickup) {
		if (!canThrow) {
			canThrow = true;
			pickup.Kill();
		}
	}
};
