#pragma once

#include "../ECS/ECS.h"
#include "../Components/InputComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/PlayerActionEvent.h"

class InputSystem : public System {
public:
	InputSystem() {
		RequireComponent<InputComponent>();
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update(std::unique_ptr<EventBus>& eventbus) {
		for (auto entity : GetSystemEntities()) {
			auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
			auto& controller = App::GetController();

			// TODO - tie speed to something that makes sense
			float speed = 0.5f;

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
			else {
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
			else {
				rigidbody.velocityY = 0.0f;
			}

			if (controller.CheckButton(XINPUT_GAMEPAD_X, true))
			{
				eventbus->EmitEvent<PlayerActionEvent>();
			}

			if (controller.CheckButton(XINPUT_GAMEPAD_B, true))
			{
				App::PlaySound(".\\TestData\\Test.wav");
			}
		}
	}
};
