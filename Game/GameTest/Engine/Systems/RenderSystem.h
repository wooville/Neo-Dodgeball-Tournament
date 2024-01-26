#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ScriptedBehaviourComponent.h"
#include "../Scripts/EnemyBehaviour.h"
#include "../Scripts/PlayerBehaviour.h"
#include "../Scripts/GameManagerBehaviour.h"
#include <algorithm>

#define AIM_LINE_SCALE_FACTOR (200.0f)

class RenderSystem : public System {
public:
	RenderSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(std::unique_ptr<Registry>& registry) {
		// organize into struct that couples transform and sprite components
		Entity gameManager = registry->GetEntityByTag("manager");
		auto& scriptedBehaviour = gameManager.GetComponent<ScriptedBehaviourComponent>();
		auto& gameManagerBehaviour = std::static_pointer_cast<GameManagerBehaviour>(scriptedBehaviour.script);

		// don't render if game over
		/*if (gameManagerBehaviour->) {

		}*/
		struct RenderableEntity {
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};
		std::vector<RenderableEntity> renderableEntities;

		// populate vector with relevant entities
		for (auto entity : GetSystemEntities()) {
			if (entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>()) {
				RenderableEntity renderableEntity;
				renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
				renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();

				// update CSimpleSprite positions to match transforms when rendering
				renderableEntity.spriteComponent.simpleSprite->SetPosition(renderableEntity.transformComponent.x, renderableEntity.transformComponent.y);

				// render lines first at this stage underneath all sprites
				if (entity.BelongsToGroup("enemies") && entity.HasComponent<ScriptedBehaviourComponent>()) {
					auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();
					auto& enemyBehaviour = std::static_pointer_cast<EnemyBehaviour>(scriptedBehaviour.script);
					Entity player = registry->GetEntityByTag("player");
					auto& playerTransform = player.GetComponent<TransformComponent>();

					if (enemyBehaviour->currentState == STATE::AIM) {
						float aimLineEndPosX = renderableEntity.transformComponent.x + enemyBehaviour->dxPlayer * (enemyBehaviour->aimTimer / 4.0f);
						float aimLineEndPosY = renderableEntity.transformComponent.y + enemyBehaviour->dyPlayer * (enemyBehaviour->aimTimer / 4.0f);

						App::DrawLine(renderableEntity.transformComponent.x, renderableEntity.transformComponent.y, aimLineEndPosX, aimLineEndPosY, 1.0, 0.0, 0.0);
					}
				}

				if (entity.HasTag("player") && entity.HasComponent<ScriptedBehaviourComponent>()) {
					auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();
					auto& playerBehaviour = std::static_pointer_cast<PlayerBehaviour>(scriptedBehaviour.script);

					if (playerBehaviour->isAiming) {
						float aimLineEndPosX = renderableEntity.transformComponent.x + playerBehaviour->throwDirectionX * (playerBehaviour->throwCharge/4.0f);
						float aimLineEndPosY = renderableEntity.transformComponent.y + playerBehaviour->throwDirectionY * (playerBehaviour->throwCharge/4.0f);

						App::DrawLine(renderableEntity.transformComponent.x, renderableEntity.transformComponent.y, aimLineEndPosX, aimLineEndPosY, 1.0, 0.0, 0.0);
					}
				}

				renderableEntities.emplace_back(renderableEntity);
			}
		}

		//sort by zIndex
		std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
			return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
		});

		// render entities
		for (auto entity : renderableEntities) {
			const auto& sprite = entity.spriteComponent;
			sprite.simpleSprite->Draw();
		}
	}
};