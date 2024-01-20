#pragma once

#include "../App/app.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/ScoreChangeEvent.h"
#include "../Components/TextComponent.h"

// timers in ms
#define TIMER_COORD_X (100.0f)
#define TIMER_COORD_Y (100.0f)
#define SCORE_COORD_X (100.0f)
#define SCORE_COORD_Y (150.0f)

class GameManagerBehaviour : public IScriptedBehaviour {
private:

public:
	float gameTimer = 0.0f;
	int score = 0;

	GameManagerBehaviour() {}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<ScoreChangeEvent>(this, &GameManagerBehaviour::onScoreChangeEvent);
	}

	void Update(Entity entity, std::unique_ptr<EventBus>& eventBus, float deltaTime) {
		gameTimer += deltaTime;

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

	//void onCollision(CollisionEvent& event) {
	//	Entity a = event.b;
	//	Entity b = event.b;
	//	
	//}
};
