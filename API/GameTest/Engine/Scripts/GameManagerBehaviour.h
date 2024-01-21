#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Components/TextComponent.h"
#include "../Scripts/EnemyBehaviour.h"

// timers in ms
#define TIMER_COORD_X (APP_VIRTUAL_WIDTH-200.0f)
#define TIMER_COORD_Y (APP_VIRTUAL_HEIGHT-50.0f)
#define SCORE_COORD_X (TIMER_COORD_X)
#define SCORE_COORD_Y (TIMER_COORD_Y-50.0f)
#define ENEMY_SPAWN_INTERVAL (30000)	// ms between each enemy spawn
#define PICKUP_SPAWN_INTERVAL (5000)	// ms between each ball spawn
#define SPAWN_COORD_X_LOWER (100.0f)
#define SPAWN_COORD_Y_LOWER (100.0f)
#define SPAWN_COORD_X_UPPER (APP_VIRTUAL_WIDTH-100.0f)
#define SPAWN_COORD_Y_UPPER (APP_VIRTUAL_HEIGHT-100.0f)

class GameManagerBehaviour : public IScriptedBehaviour {
private:

public:
	float gameTimer = 0.0f;
	float enemySpawnTimer = ENEMY_SPAWN_INTERVAL;
	float pickupSpawnTimer = PICKUP_SPAWN_INTERVAL;
	int score = 0;
	int currentBalls = 0;
	int maxBalls = 3;

	GameManagerBehaviour() {}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<ScoreChangeEvent>(this, &GameManagerBehaviour::onScoreChangeEvent);
	}

	void Update(Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		gameTimer += deltaTime;
		enemySpawnTimer += deltaTime;
		pickupSpawnTimer += deltaTime;

		if (enemySpawnTimer > ENEMY_SPAWN_INTERVAL) {
			// spawn enemy at random spot
			SpawnEnemy(entity);
			enemySpawnTimer = 0.0f;
		}

		if (pickupSpawnTimer > PICKUP_SPAWN_INTERVAL && currentBalls < maxBalls) {
			SpawnPickup(entity);
			pickupSpawnTimer = 0.0f;
		}

		auto& textComponent = entity.GetComponent<TextComponent>();
		UpdateTextComponent(textComponent);
	}

	void UpdateTextComponent(TextComponent& textComponent) {
		std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;
		std::pair<float, float> coords = std::make_pair(TIMER_COORD_X, TIMER_COORD_Y);
		textToRender.emplace_back(coords, std::to_string(gameTimer));
		coords = std::make_pair(SCORE_COORD_X, SCORE_COORD_Y);
		textToRender.emplace_back(coords, std::to_string(score));
		textComponent.textToRender = textToRender;
	}

	void onScoreChangeEvent(ScoreChangeEvent& event) {
		score += event.deltaScore;
	}

	void SpawnEnemy(Entity entity){
		Entity newEnemy = entity.registry->CreateEntity();
		float spawnX = SPAWN_COORD_X_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_X_UPPER - SPAWN_COORD_X_LOWER)));
		float spawnY = SPAWN_COORD_Y_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_Y_UPPER - SPAWN_COORD_Y_LOWER)));
		newEnemy.AddComponent<TransformComponent>(spawnX, spawnY);
		newEnemy.AddComponent<SpriteComponent>(".\\TestData\\red_square.bmp", 1, 1);
		newEnemy.AddComponent<AnimationComponent>();
		newEnemy.AddComponent<RigidBodyComponent>();
		newEnemy.AddComponent<BoxColliderComponent>(32, 32);
		newEnemy.AddComponent<HealthComponent>(20);
		newEnemy.AddComponent<ProjectileEmitterComponent>(0.7, 0.7, 0, 3000, 10, false);
		newEnemy.AddComponent<ScriptedBehaviourComponent>(std::make_shared<EnemyBehaviour>());
		newEnemy.Group("enemies");
	}

	void SpawnPickup(Entity entity) {
		Entity newPickup = entity.registry->CreateEntity();
		//std::uniform_int_distribution<std::mt19937::result_type> dist6(100, static_cast<int>(APP_VIRTUAL_WIDTH-100.0));
		float spawnX = SPAWN_COORD_X_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_X_UPPER - SPAWN_COORD_X_LOWER)));
		//dist6(100, static_cast<int>(APP_VIRTUAL_HEIGHT - 100.0));
		float spawnY = SPAWN_COORD_Y_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_Y_UPPER - SPAWN_COORD_Y_LOWER)));
		newPickup.AddComponent<TransformComponent>(spawnX, spawnY);
		newPickup.AddComponent<SpriteComponent>(".\\TestData\\green_square.bmp", 1, 1, 0);
		newPickup.AddComponent<BoxColliderComponent>(32, 32);
		newPickup.Group("pickups");
	}
};
