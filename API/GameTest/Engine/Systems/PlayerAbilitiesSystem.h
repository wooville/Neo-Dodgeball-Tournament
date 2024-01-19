#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/PlayerActionEvent.h"
#include "../Events/CollisionEvent.h"
//#include "../Events/PickupEvent.h"
#include "../Components/PlayerAbilitiesComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class PlayerAbilitiesSystem : public System {
public:
	PlayerAbilitiesSystem() {
		RequireComponent<PlayerAbilitiesComponent>();
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		//eventBus->SubscribeToEvent<PickupEvent>();
		eventBus->SubscribeToEvent<CollisionEvent>(this, &PlayerAbilitiesSystem::onCollision);
		//eventBus->SubscribeToEvent<PickupEvent>(this, &PlayerAbilitiesSystem::onPickup);
	}

	bool canThrow = false;

	void Update(std::unique_ptr<EventBus>& eventBus) {
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

			if (canThrow && controller.CheckButton(XINPUT_GAMEPAD_X, true))
			{
				eventBus->EmitEvent<PlayerActionEvent>();
				canThrow = false;
			}

			if (controller.CheckButton(XINPUT_GAMEPAD_B, true))
			{
				App::PlaySound(".\\TestData\\Test.wav");
			}
		}
	}

	void onCollision(CollisionEvent& event) {
		Entity a = event.b;
		Entity b = event.b;
		if (a.BelongsToGroup("pickups")) {
			onPickup(a);
		}
		if (b.BelongsToGroup("pickups")) {
			onPickup(a);
		}
	}

	void onPickup(Entity pickup) {
		if (!canThrow) {
			canThrow = true;
			pickup.Kill();
		}
	}
};
