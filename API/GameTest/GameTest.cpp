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
//#include "Engine/Systems/PlayerAbilitiesSystem.h"
//#include "Engine/Systems/KeyboardControlSystem.h"
//#include "Engine/Systems/CameraMovementSystem.h"
#include "Engine/Systems/ProjectileEmitSystem.h"
#include "Engine/Systems/ProjectileLifecycleSystem.h"
#include "Engine/Systems/RenderTextSystem.h"
//#include "Engine/Systems/RenderHealthBarSystem.h"
//#include "Engine/Systems/RenderGUISystem.h"
#include "Engine/Systems/ScriptedBehaviourSystem.h"

#include "Engine/Scripts/GameManagerBehaviour.h"
#include "Engine/Scripts/PlayerBehaviour.h"
//------------------------------------------------------------------------
std::unique_ptr<Registry> registry;
std::unique_ptr<EventBus> eventBus;
//std::unique_ptr<AssetStore> assetStore;

float gameTimer;
int score;

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

	//------------------------------------------------------------------------
	Entity player = registry->CreateEntity();
	player.AddComponent<TransformComponent>(400.0f, 400.0f);
	player.AddComponent<SpriteComponent>(".\\TestData\\red_square.bmp", 1, 1, 1);
	player.AddComponent<AnimationComponent>();
	player.AddComponent<RigidBodyComponent>();
	player.AddComponent<BoxColliderComponent>(32,32);
	//player.AddComponent<PlayerAbilitiesComponent>();
	player.AddComponent<HealthComponent>(200);
	player.AddComponent<ProjectileEmitterComponent>(0.7, 0.7, 0, 1000, 10, true);

	//PlayerBehaviour* playerBehaviour = new PlayerBehaviour();
	//PlayerBehaviour* playerBehaviour = new PlayerBehaviour();
	//std::shared_ptr<PlayerBehaviour> playerBehaviour = ;
	player.AddComponent<ScriptedBehaviourComponent>(std::make_shared<PlayerBehaviour>());
	//player.GetComponent<SpriteComponent>().simpleSprite->SetScale(5.0f);
	player.Tag("player");	// tags are unique, one entity per tag

	Entity npc = registry->CreateEntity();
	npc.AddComponent<TransformComponent>(600.0f, 600.0f);
	npc.AddComponent<SpriteComponent>(".\\TestData\\Test.bmp", 8, 4);
	npc.AddComponent<AnimationComponent>();
	npc.AddComponent<RigidBodyComponent>();
	npc.AddComponent<BoxColliderComponent>(140,140);
	npc.AddComponent<HealthComponent>(20);
	npc.AddComponent<ProjectileEmitterComponent>(-0.5, 0, 1000);
	npc.Group("enemies");	// groups are not unique, multiple entities per group

	Entity pickup = registry->CreateEntity();
	pickup.AddComponent<TransformComponent>(600.0f, 400.0f);
	pickup.AddComponent<SpriteComponent>(".\\TestData\\blue_square.bmp", 1, 1, 0);
	pickup.AddComponent<BoxColliderComponent>(32, 32);
	pickup.Group("pickups");

	Entity pickup2 = registry->CreateEntity();
	pickup2.AddComponent<TransformComponent>(800.0f, 400.0f);
	pickup2.AddComponent<SpriteComponent>(".\\TestData\\blue_square.bmp", 1, 1, 0);
	pickup2.AddComponent<BoxColliderComponent>(32, 32);
	pickup2.Group("pickups");

	Entity gameManager = registry->CreateEntity();
	std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;
	std::pair<float, float> coords = std::make_pair(300.0, 200.0);
	textToRender.emplace_back(coords, "test");
	coords = std::make_pair(500.0, 300.0);
	textToRender.emplace_back(coords, "test2");
	gameManager.AddComponent<TextComponent>();
	gameManager.AddComponent<ScriptedBehaviourComponent>(std::make_shared<GameManagerBehaviour>());

	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	//registry->AddSystem<RenderColliderSystem>();
	registry->AddSystem<DamageSystem>();
	//registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifecycleSystem>();
	registry->AddSystem<RenderTextSystem>();
	//registry->AddSystem<RenderHealthBarSystem>();
	//registry->AddSystem<RenderGUISystem>();
	registry->AddSystem<ScriptedBehaviourSystem>();

	gameTimer = 0.0;
	score = 0;
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
	eventBus->Reset();

	// subscribe to events for all systems for current frame
	registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ScriptedBehaviourSystem>().SubscribeToEvents(eventBus);

	// update registry to process entities
	registry->Update();

	// invoke systems that need to update
	//registry->GetSystem<PlayerAbilitiesSystem>().Update(eventBus, deltaTime);
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update(deltaTime);
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	//registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<ProjectileLifecycleSystem>().Update();
	registry->GetSystem<ScriptedBehaviourSystem>().Update(eventBus, deltaTime);

	gameTimer += deltaTime;
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	registry->GetSystem<RenderSystem>().Update();
	registry->GetSystem<RenderTextSystem>().Update();

	App::Print(APP_VIRTUAL_WIDTH - 200, 150, "LT - Aim");

	//------------------------------------------------------------------------
	// Example Line Drawing.
	//------------------------------------------------------------------------
	//static float a = 0.0f;
	//float r = 1.0f;
	//float g = 1.0f;
	//float b = 1.0f;
	//a += 0.1f;
	//for (int i = 0; i < 20; i++)
	//{

	//	float sx = 200 + sinf(a + i * 0.1f)*60.0f;
	//	float sy = 200 + cosf(a + i * 0.1f)*60.0f;
	//	float ex = 700 - sinf(a + i * 0.1f)*60.0f;
	//	float ey = 700 - cosf(a + i * 0.1f)*60.0f;
	//	g = (float)i / 20.0f;
	//	b = (float)i / 20.0f;
	//	App::DrawLine(sx, sy, ex, ey,r,g,b);
	//}

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