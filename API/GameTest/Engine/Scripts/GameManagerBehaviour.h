#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Components/TextComponent.h"
#include "../Scripts/EnemyBehaviour.h"

// timers in ms
#define TIMER_COORD_X (100.0f)
#define TIMER_COORD_Y (100.0f)
#define SCORE_COORD_X (100.0f)
#define SCORE_COORD_Y (150.0f)
#define ENEMY_SPAWN_INTERVAL (5000)	// ms between each enemy spawn
#define ENEMY_SPAWN_COORD_X_LOWER (100.0f)
#define ENEMY_SPAWN_COORD_Y_LOWER (100.0f)
#define ENEMY_SPAWN_COORD_X_UPPER (APP_VIRTUAL_WIDTH-100.0f)
#define ENEMY_SPAWN_COORD_Y_UPPER (APP_VIRTUAL_HEIGHT-100.0f)

class GameManagerBehaviour : public IScriptedBehaviour {
private:

public:
	float gameTimer = 0.0f;
	float enemySpawnTimer = ENEMY_SPAWN_INTERVAL;
	int score = 0;

	GameManagerBehaviour() {}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<ScoreChangeEvent>(this, &GameManagerBehaviour::onScoreChangeEvent);
	}

	void Update(std::unique_ptr<Registry>& registry, Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		gameTimer += deltaTime;
		enemySpawnTimer += deltaTime;

		if (enemySpawnTimer > ENEMY_SPAWN_INTERVAL) {
			
			// spawn enemy at random spot
			SpawnEnemy(registry);
			enemySpawnTimer = 0;
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

	void SpawnEnemy(std::unique_ptr<Registry>& registry){
		Entity newEnemy = registry->CreateEntity();
		//std::uniform_int_distribution<std::mt19937::result_type> dist6(100, static_cast<int>(APP_VIRTUAL_WIDTH-100.0));
		float spawnX = ENEMY_SPAWN_COORD_X_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (ENEMY_SPAWN_COORD_X_UPPER - ENEMY_SPAWN_COORD_X_LOWER)));
		//dist6(100, static_cast<int>(APP_VIRTUAL_HEIGHT - 100.0));
		float spawnY = ENEMY_SPAWN_COORD_Y_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (ENEMY_SPAWN_COORD_Y_UPPER - ENEMY_SPAWN_COORD_Y_LOWER)));
		newEnemy.AddComponent<TransformComponent>(spawnX, spawnY);
		newEnemy.AddComponent<SpriteComponent>(".\\TestData\\Test.bmp", 8, 4);
		newEnemy.AddComponent<AnimationComponent>();
		newEnemy.AddComponent<RigidBodyComponent>();
		newEnemy.AddComponent<BoxColliderComponent>(140, 140);
		newEnemy.AddComponent<HealthComponent>(20);
		newEnemy.AddComponent<ProjectileEmitterComponent>(-0.5, 0, 1000);
		newEnemy.AddComponent<ScriptedBehaviourComponent>(std::make_shared<EnemyBehaviour>());
		newEnemy.Group("enemies");
	}

	//void onCollision(CollisionEvent& event) {
	//	Entity a = event.b;
	//	Entity b = event.b;
	//	
	//}
};
