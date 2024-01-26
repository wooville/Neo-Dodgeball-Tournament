//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
//------------------------------------------------------------------------
#include "app\app.h"
//------------------------------------------------------------------------
#include "Engine/ECS/ECS.h"
#include "Engine/Systems/MovementSystem.h"
#include "Engine/Systems/RenderSystem.h"
#include "Engine/Systems/AnimationSystem.h"
#include "Engine/Systems/CollisionSystem.h"
#include "Engine/Systems/CollisionListenerSystem.h"
#include "Engine/Systems/ProjectileEmitSystem.h"
#include "Engine/Systems/ProjectileLifecycleSystem.h"
#include "Engine/Systems/RenderTextSystem.h"
#include "Engine/Systems/ScriptedBehaviourSystem.h"

#include "Engine/Scripts/GameManagerBehaviour.h"
#include "Engine/Scripts/PlayerBehaviour.h"
//------------------------------------------------------------------------
#define ROUNDS_FILE_PATH (".\\Data\\rounds.txt")

std::unique_ptr<Registry> registry;
std::unique_ptr<EventBus> eventBus;

void InitECS() {
	srand(static_cast <unsigned> (time(0)));
	registry = std::make_unique<Registry>();
	eventBus = std::make_unique<EventBus>();

	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<CollisionListenerSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifecycleSystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<ScriptedBehaviourSystem>();
}

// read in rounds data
void InitGameManager() {
	std::ifstream infile(ROUNDS_FILE_PATH);

	std::vector<std::pair<int, int>> rounds;
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		int enemies, balls;
		if (!(iss >> enemies >> balls)) { break; } // error

		// process pair (a,b)
		rounds.push_back(std::make_pair(enemies, balls));
	}

	Entity gameManager = registry->CreateEntity();
	gameManager.AddComponent<TextComponent>();
	gameManager.AddComponent<ScriptedBehaviourComponent>(std::make_shared<GameManagerBehaviour>(rounds));
	gameManager.Tag("manager");
}

void InitPlayer() {
	Entity player = registry->CreateEntity();
	player.AddComponent<TransformComponent>(SPAWN_COORD_X, SPAWN_COORD_Y);
	player.AddComponent<SpriteComponent>(".\\Data\\Sprites\\blue_square.bmp", 1, 1, 1);
	player.AddComponent<AnimationComponent>();
	player.AddComponent<RigidBodyComponent>();
	player.AddComponent<BoxColliderComponent>(32, 32);
	player.AddComponent<HealthComponent>(5);
	player.AddComponent<ProjectileEmitterComponent>(0.7, 0.7, 0, 3000, 1, true);
	player.AddComponent<TextComponent>();
	player.AddComponent<ScriptedBehaviourComponent>(std::make_shared<PlayerBehaviour>());
	player.Tag("player");	// tags are unique, one entity per tag
}

void InitWalls() {
	// boundaries for ball collision
	Entity wallTop = registry->CreateEntity();
	wallTop.AddComponent<TransformComponent>(0.0f, APP_VIRTUAL_HEIGHT - 2);
	wallTop.AddComponent<BoxColliderComponent>(APP_VIRTUAL_WIDTH, 2);
	wallTop.Group("walls");
	wallTop.Group("reverseY");

	Entity wallRight = registry->CreateEntity();
	wallRight.AddComponent<TransformComponent>(APP_VIRTUAL_WIDTH - 2, 0.0f);
	wallRight.AddComponent<BoxColliderComponent>(2, APP_VIRTUAL_HEIGHT);
	wallRight.Group("walls");
	wallRight.Group("reverseX");

	Entity wallBottom = registry->CreateEntity();
	wallBottom.AddComponent<TransformComponent>(0.0f, 0.0f);
	wallBottom.AddComponent<BoxColliderComponent>(APP_VIRTUAL_WIDTH, 32);
	wallBottom.Group("walls");
	wallBottom.Group("reverseY");

	Entity wallLeft = registry->CreateEntity();
	wallLeft.AddComponent<TransformComponent>(0.0f, 0.0f);
	wallLeft.AddComponent<BoxColliderComponent>(32, APP_VIRTUAL_HEIGHT);
	wallLeft.Group("walls");
	wallLeft.Group("reverseX");
}

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	//------------------------------------------------------------------------
	InitECS();
	InitGameManager();
	InitPlayer();
	InitWalls();
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
	eventBus->Reset();

	// subscribe to events for all systems for current frame
	registry->GetSystem<CollisionListenerSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ScriptedBehaviourSystem>().SubscribeToEvents(eventBus);

	// update registry to process entities
	registry->Update();

	Entity gameManager = registry->GetEntityByTag("manager");
	auto& scriptedBehaviour = gameManager.GetComponent<ScriptedBehaviourComponent>();
	auto& gameManagerBehaviour = std::static_pointer_cast<GameManagerBehaviour>(scriptedBehaviour.script);

	// invoke systems that need to update
	// if the game is not active, only update scripted behaviours
	 registry->GetSystem<ScriptedBehaviourSystem>().Update(eventBus, deltaTime);
	if (gameManagerBehaviour->gameState == GAME_STATE::ACTIVE) {
		registry->GetSystem<MovementSystem>().Update(deltaTime);
		registry->GetSystem<AnimationSystem>().Update(deltaTime);
		registry->GetSystem<CollisionSystem>().Update(eventBus);
		registry->GetSystem<ProjectileEmitSystem>().Update(registry);
		registry->GetSystem<ProjectileLifecycleSystem>().Update();
	}
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	registry->GetSystem<RenderSystem>().Update(registry);
	registry->GetSystem<RenderTextSystem>().Update();
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