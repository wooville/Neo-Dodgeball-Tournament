//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
#include <memory>
//------------------------------------------------------------------------
#include "app\app.h"
//------------------------------------------------------------------------
#include "Engine/ECS/ECS.h"
//#include "Engine/AssetStore/AssetStore.h"
#include "Engine/Systems/MovementSystem.h"
#include "Engine/Systems/RenderSystem.h"
#include "Engine/Systems/AnimationSystem.h"
#include "Engine/Systems/CollisionSystem.h"
//#include "Engine/Systems/RenderColliderSystem.h"
#include "Engine/Systems/DamageSystem.h"
#include "Engine/Systems/InputSystem.h"
//#include "Engine/Systems/KeyboardControlSystem.h"
//#include "Engine/Systems/CameraMovementSystem.h"
#include "Engine/Systems/ProjectileEmitSystem.h"
#include "Engine/Systems/ProjectileLifecycleSystem.h"
//#include "Engine/Systems/RenderTextSystem.h"
//#include "Engine/Systems/RenderHealthBarSystem.h"
//#include "Engine/Systems/RenderGUISystem.h"
//#include "Engine/Systems/ScriptSystem.h"
//------------------------------------------------------------------------
// Eample data....
//------------------------------------------------------------------------
//CSimpleSprite *testSprite;
//enum
//{
//	ANIM_FORWARDS,
//	ANIM_BACKWARDS,
//	ANIM_LEFT,
//	ANIM_RIGHT,
//};
//------------------------------------------------------------------------
std::unique_ptr<Registry> registry;
std::unique_ptr<EventBus> eventBus;
//std::unique_ptr<AssetStore> assetStore;

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	//isRunning = false;
	//isDebug = false;
	registry = std::make_unique<Registry>();
	//assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();
	//Logger::Log("Game constructor called.");

	//-----------------------------------------
	//testSprite = App::CreateSprite(".\\TestData\\Test.bmp", 8, 4);
	//testSprite->SetPosition(400.0f, 400.0f);
	//float speed = 1.0f / 15.0f;
	//testSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
	//testSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
	//testSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
	//testSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
	//testSprite->SetScale(1.0f);
	//------------------------------------------------------------------------
	Entity player = registry->CreateEntity();
	player.AddComponent<TransformComponent>(400.0f, 400.0f);
	player.AddComponent<SpriteComponent>(".\\TestData\\sq.bmp", 1, 1, 1);
	player.AddComponent<AnimationComponent>();
	player.AddComponent<RigidBodyComponent>();
	player.AddComponent<BoxColliderComponent>(32,32);
	player.AddComponent<InputComponent>();
	player.AddComponent<HealthComponent>(20);
	player.AddComponent<ProjectileEmitterComponent>();
	//player.GetComponent<SpriteComponent>().simpleSprite->SetScale(5.0f);
	player.Tag("player");

	Entity npc = registry->CreateEntity();
	npc.AddComponent<TransformComponent>(600.0f, 600.0f);
	npc.AddComponent<SpriteComponent>(".\\TestData\\Test.bmp", 8, 4);
	npc.AddComponent<AnimationComponent>();
	npc.AddComponent<RigidBodyComponent>();
	npc.AddComponent<BoxColliderComponent>(140,140);
	npc.AddComponent<HealthComponent>(20);
	npc.AddComponent<ProjectileEmitterComponent>(-0.5, 0, 1000);
	npc.Group("enemies");

	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<InputSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	//registry->AddSystem<RenderColliderSystem>();
	registry->AddSystem<DamageSystem>();
	//registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifecycleSystem>();
	//registry->AddSystem<RenderTextSystem>();
	//registry->AddSystem<RenderHealthBarSystem>();
	//registry->AddSystem<RenderGUISystem>();
	//registry->AddSystem<ScriptSystem>();
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
	//------------------------------------------------------------------------
	// Example Sprite Code....
	//testSprite->Update(deltaTime);
	
	//if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_UP, false))
	//{
	//	testSprite->SetScale(testSprite->GetScale() + 0.1f);
	//}
	//if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_DOWN, false))
	//{
	//	testSprite->SetScale(testSprite->GetScale() - 0.1f);
	//}
	//if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_LEFT, false))
	//{
	//	testSprite->SetAngle(testSprite->GetAngle() + 0.1f);
	//}
	//if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_RIGHT, false))
	//{
	//	testSprite->SetAngle(testSprite->GetAngle() - 0.1f);
	//}
	//if (App::GetController().CheckButton(XINPUT_GAMEPAD_A, true))
	//{
	//	testSprite->SetAnimation(-1);
	//}

	eventBus->Reset();

	// subscribe to events for all systems for current frame
	registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

	// update registry to process entities
	registry->Update();

	// invoke systems that need to update
	registry->GetSystem<InputSystem>().Update(eventBus);
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update(deltaTime);
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	//registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<ProjectileLifecycleSystem>().Update();
	//registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	registry->GetSystem<RenderSystem>().Update();

	//------------------------------------------------------------------------
	// Example Text.
	//------------------------------------------------------------------------
	//App::Print(100, 100, "Sample Text");

	//------------------------------------------------------------------------
	// Example Line Drawing.
	//------------------------------------------------------------------------
	static float a = 0.0f;
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	a += 0.1f;
	for (int i = 0; i < 20; i++)
	{

		float sx = 200 + sinf(a + i * 0.1f)*60.0f;
		float sy = 200 + cosf(a + i * 0.1f)*60.0f;
		float ex = 700 - sinf(a + i * 0.1f)*60.0f;
		float ey = 700 - cosf(a + i * 0.1f)*60.0f;
		g = (float)i / 20.0f;
		b = (float)i / 20.0f;
		App::DrawLine(sx, sy, ex, ey,r,g,b);
	}

	// TODO - draw colliders
	//App::DrawLine();
}
//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{	
	//------------------------------------------------------------------------
	// Example Sprite Code....
	//delete testSprite;
	//------------------------------------------------------------------------
}