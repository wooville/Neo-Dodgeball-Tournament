#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Events/ImportantEntityDiedEvent.h"
#include "../Events/ResetGameEvent.h"
#include "../Components/TextComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Scripts/EnemyBehaviour.h"

// timers in ms
#define MAIN_MESSAGE_COORD_X (APP_VIRTUAL_WIDTH/2)
#define MAIN_MESSAGE_COORD_Y (APP_VIRTUAL_HEIGHT/2)
#define ROUND_COORD_X (50.0f)
#define ROUND_COORD_Y (APP_VIRTUAL_HEIGHT-50.0f)
#define TIMER_COORD_X (APP_VIRTUAL_WIDTH-200.0f)
#define TIMER_COORD_Y (APP_VIRTUAL_HEIGHT-50.0f)
#define SCORE_COORD_X (TIMER_COORD_X)
#define SCORE_COORD_Y (TIMER_COORD_Y-50.0f)
#define ROUND_START_INTERVAL (2000)	// ms between each round
#define SPAWN_COORD_X_LOWER (100.0f)
#define SPAWN_COORD_Y_LOWER (100.0f)
#define SPAWN_COORD_X_UPPER (APP_VIRTUAL_WIDTH-100.0f)
#define SPAWN_COORD_Y_UPPER (APP_VIRTUAL_HEIGHT-100.0f)
#define ENEMY_SCORE (10)
#define ROUND_START_TIME (3000.0f)


enum GAME_STATE {
	ACTIVE,
	PAUSED,
	GAME_LOST,
	GAME_WON
};

class GameManagerBehaviour : public IScriptedBehaviour {
private:
	std::vector<std::pair<int, int>> rounds;

public:
	GAME_STATE gameState = GAME_STATE::ACTIVE;
	float gameTimer = 0.0f;
	float roundStartTimer = 0.0f;
	float roundTimer = 0.0f;
	int score = 0;
	int currentEnemies = 0;
	int maxEnemies = 3;
	int currentBalls = 0;
	int maxBalls = 3;
	int currentRound = 0;
	bool roundStarted = false;

	GameManagerBehaviour(std::vector<std::pair<int,int>> rounds) {
		this->rounds = rounds;
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<ResetGameEvent>(this, &GameManagerBehaviour::onResetGameEvent);
		eventBus->SubscribeToEvent<ScoreChangeEvent>(this, &GameManagerBehaviour::onScoreChangeEvent);
		eventBus->SubscribeToEvent<ImportantEntityDiedEvent>(this, &GameManagerBehaviour::onImportantEntityDiedEvent);
	}

	void Update(Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		maxEnemies = rounds[currentRound].first;
		maxBalls = rounds[currentRound].second;
		currentBalls = 0;
		

		// check total number of balls in play
		if (entity.registry->GetNumberOfEntitiesInGroup("pickups") > 0) {
			std::vector<Entity> pickups = entity.registry->GetEntitiesByGroup("pickups");
			currentBalls += pickups.size();
		}
		
		if (entity.registry->GetNumberOfEntitiesInGroup("projectiles") > 0) {
			std::vector<Entity> projectiles = entity.registry->GetEntitiesByGroup("projectiles");
			currentBalls += projectiles.size();
		}

		// account for balls that players and enemies may be holding
		auto& scriptedBehaviour = entity.registry->GetEntityByTag("player").GetComponent<ScriptedBehaviourComponent>();
		auto& playerBehaviour = std::static_pointer_cast<PlayerBehaviour>(scriptedBehaviour.script);
		if (playerBehaviour->canThrow) {
			currentBalls++;
		}

		if (entity.registry->GetNumberOfEntitiesInGroup("enemies") > 0) {
			for (Entity e : entity.registry->GetEntitiesByGroup("enemies")) {
				auto& scriptedBehaviour = e.GetComponent<ScriptedBehaviourComponent>();
				auto& enemyBehaviour = std::static_pointer_cast<EnemyBehaviour>(scriptedBehaviour.script);
				if (enemyBehaviour->canThrow) {
					currentBalls++;
				}
			}
		}

		// pause non-scripted behaviours until round starts
		// spawn enemies and balls for this round
		if (!roundStarted) {
			gameState = GAME_STATE::PAUSED;
			roundTimer += deltaTime;
			
			while (currentEnemies < maxEnemies) {
				// spawn enemy at random spot
				SpawnEnemy(entity);
			}
			while (currentBalls < maxBalls) {
				// spawn ball at random spot
				SpawnPickup(entity);
			}
		}

		if (!roundStarted && roundStartTimer > ROUND_START_INTERVAL) {
			roundStarted = true;
			roundStartTimer = 0.0f;
			gameState = GAME_STATE::ACTIVE;
		}

		/*if (pickupSpawnTimer > PICKUP_SPAWN_INTERVAL && currentBalls < maxBalls) {
			SpawnPickup(entity);
			pickupSpawnTimer = 0.0f;
		}*/

		// if all enemies are eliminated, start the next round
		// if that was the final round, player wins
		if (currentEnemies == 0 && currentRound < rounds.size()-1) {
			currentRound++;
			roundStarted = false;
			roundStartTimer = 0.0f;
		}
		else if (currentRound == rounds.size() - 1) {
			EndGame(true);
		}

		auto& textComponent = entity.GetComponent<TextComponent>();
		if (gameState == GAME_STATE::GAME_WON) {
			UpdateTextWon(textComponent);
		}
		else if (gameState == GAME_STATE::GAME_LOST) {
			UpdateTextLost(textComponent);
		}
		else {
			UpdateTextActive(textComponent);
		}

		roundStartTimer += deltaTime;
		if (gameState == GAME_STATE::ACTIVE) {
			gameTimer += deltaTime;
		}
	}

	void UpdateTextWon(TextComponent& textComponent) {
		std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;

		std::pair<float, float> coords = std::make_pair(MAIN_MESSAGE_COORD_X, MAIN_MESSAGE_COORD_Y);
		textToRender.emplace_back(coords, "YOU WIN");

		coords = std::make_pair(MAIN_MESSAGE_COORD_X, MAIN_MESSAGE_COORD_Y);
		textToRender.emplace_back(coords, "PRESS A TO PLAY AGAIN");

		textComponent.textToRender = textToRender;
	}

	void UpdateTextLost(TextComponent& textComponent) {
		std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;

		std::pair<float, float> coords = std::make_pair(MAIN_MESSAGE_COORD_X, MAIN_MESSAGE_COORD_Y);
		textToRender.emplace_back(coords, "GAME OVER");

		coords = std::make_pair(MAIN_MESSAGE_COORD_X, MAIN_MESSAGE_COORD_Y-50.0f);
		textToRender.emplace_back(coords, "PRESS A TO PLAY AGAIN");

		textComponent.textToRender = textToRender;
	}

	void UpdateTextActive(TextComponent& textComponent) {
		std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;

		std::pair<float, float> coords = std::make_pair(ROUND_COORD_X, ROUND_COORD_Y);
		textToRender.emplace_back(coords, "ROUND " + std::to_string(currentRound+1));

		coords = std::make_pair(TIMER_COORD_X, TIMER_COORD_Y);
		textToRender.emplace_back(coords, std::to_string(gameTimer));

		coords = std::make_pair(SCORE_COORD_X, SCORE_COORD_Y);
		textToRender.emplace_back(coords, "SCORE " + std::to_string(score));

		textComponent.textToRender = textToRender;
	}

	void onScoreChangeEvent(ScoreChangeEvent& event) {
		score += event.deltaScore;
	}

	void onImportantEntityDiedEvent(ImportantEntityDiedEvent& event) {
		Entity e = event.entity;
		if (e.HasTag("player")) {
			EndGame(false);
		}
		else if (e.BelongsToGroup("enemies")) {
			score += ENEMY_SCORE;
			currentEnemies--;
		} 
	}

	void onResetGameEvent(ResetGameEvent& event) {
		Entity e = event.a;

		// remove all entities except for player
		if (e.registry->GetNumberOfEntitiesInGroup("pickups") > 0) {
			for (Entity e : e.registry->GetEntitiesByGroup("pickups")) {
				e.Kill();
			}
		}

		if (e.registry->GetNumberOfEntitiesInGroup("projectiles") > 0) {
			for (Entity e : e.registry->GetEntitiesByGroup("projectiles")) {
				e.Kill();
			}
		}

		if (e.registry->GetNumberOfEntitiesInGroup("enemies") > 0) {
			for (Entity e : e.registry->GetEntitiesByGroup("enemies")) {
				e.Kill();
			}
		}

		currentRound = 0;
		currentEnemies = 0;
		currentBalls = 0;
		roundStarted = false;
		roundTimer = 0.0f;
		gameTimer = 0.0f;
		score = 0;
		gameState = GAME_STATE::ACTIVE;
	}

	void SpawnEnemy(Entity entity){
		Entity newEnemy = entity.registry->CreateEntity();
		float spawnX = SPAWN_COORD_X_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_X_UPPER - SPAWN_COORD_X_LOWER)));
		float spawnY = SPAWN_COORD_Y_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_Y_UPPER - SPAWN_COORD_Y_LOWER)));
		newEnemy.AddComponent<TransformComponent>(spawnX, spawnY);
		newEnemy.AddComponent<SpriteComponent>(".\\Data\\Sprites\\red_square.bmp", 1, 1);
		newEnemy.AddComponent<RigidBodyComponent>();
		newEnemy.AddComponent<BoxColliderComponent>(32, 32);
		newEnemy.AddComponent<HealthComponent>(20);
		newEnemy.AddComponent<ProjectileEmitterComponent>(0.7, 0.7, 0, 3000, 10, false);
		newEnemy.AddComponent<ScriptedBehaviourComponent>(std::make_shared<EnemyBehaviour>());
		newEnemy.Group("enemies");
		currentEnemies++;
	}

	void SpawnPickup(Entity entity) {
		Entity newPickup = entity.registry->CreateEntity();
		float spawnX = SPAWN_COORD_X_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_X_UPPER - SPAWN_COORD_X_LOWER)));
		float spawnY = SPAWN_COORD_Y_LOWER + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (SPAWN_COORD_Y_UPPER - SPAWN_COORD_Y_LOWER)));
		newPickup.AddComponent<TransformComponent>(spawnX, spawnY);
		newPickup.AddComponent<SpriteComponent>(".\\Data\\Sprites\\green_square.bmp", 1, 1, 0);
		newPickup.AddComponent<BoxColliderComponent>(32, 32);
		newPickup.Group("pickups");
		currentBalls++;
	}

	void EndGame(bool playerWon) {
		if (playerWon) {
			gameState = GAME_STATE::GAME_WON;
		}
		else {
			gameState = GAME_STATE::GAME_LOST;
		}
	}
};
