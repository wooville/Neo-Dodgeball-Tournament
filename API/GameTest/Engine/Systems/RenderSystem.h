#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ScriptedBehaviourComponent.h"
#include "../Scripts/EnemyBehaviour.h"
#include "../Scripts/PlayerBehaviour.h"
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

					//if (enemyBehaviour->isAiming) {
						float aimLineEndPosX = renderableEntity.transformComponent.x + enemyBehaviour->dx*AIM_LINE_SCALE_FACTOR;
						float aimLineEndPosY = renderableEntity.transformComponent.y + enemyBehaviour->dy*AIM_LINE_SCALE_FACTOR;

						App::DrawLine(renderableEntity.transformComponent.x, renderableEntity.transformComponent.y, aimLineEndPosX, aimLineEndPosY, 1.0, 0.0, 0.0);
					//}
				}

				if (entity.HasTag("player") && entity.HasComponent<ScriptedBehaviourComponent>()) {
					auto& scriptedBehaviour = entity.GetComponent<ScriptedBehaviourComponent>();
					auto& playerBehaviour = std::static_pointer_cast<PlayerBehaviour>(scriptedBehaviour.script);

					if (playerBehaviour->isAiming) {
						float aimLineEndPosX = renderableEntity.transformComponent.x + playerBehaviour->throwDirectionX * AIM_LINE_SCALE_FACTOR;
						float aimLineEndPosY = renderableEntity.transformComponent.y + playerBehaviour->throwDirectionY * AIM_LINE_SCALE_FACTOR;

						App::DrawLine(renderableEntity.transformComponent.x, renderableEntity.transformComponent.y, aimLineEndPosX, aimLineEndPosY, 1.0, 0.0, 0.0);
					}
				}
				

				//// don't bother rendering entities outside of camera
				//bool isEntityOutsideCameraView = (
				//	renderableEntity.transformComponent.position.x + (renderableEntity.transformComponent.scale.x * renderableEntity.spriteComponent.width) < camera.x ||
				//	renderableEntity.transformComponent.position.x  > camera.x + camera.w ||
				//	renderableEntity.transformComponent.position.y + (renderableEntity.transformComponent.scale.y * renderableEntity.spriteComponent.height) < camera.y ||
				//	renderableEntity.transformComponent.position.y > camera.y + camera.h
				//);

				/*if (isEntityOutsideCameraView && !renderableEntity.spriteComponent.isFixed) {
					continue;
				}*/

				renderableEntities.emplace_back(renderableEntity);
			}
		}

		//sort by zIndex
		std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
			return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
		});

		// render entities
		for (auto entity : renderableEntities) {
			// update position based on velocity
			const auto& transform = entity.transformComponent;
			const auto& sprite = entity.spriteComponent;
			sprite.simpleSprite->Draw();

			//// rectangle to carve out of original sprite texture
			//SDL_Rect srcRect = sprite.srcRect;

			//// where to draw entity on map
			//SDL_Rect dstRect = {
			//	static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
			//	static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
			//	static_cast<int>(sprite.width * transform.scale.x),
			//	static_cast<int>(sprite.height * transform.scale.y)
			//};

			//SDL_RenderCopyEx(
			//	renderer,
			//	assetStore->GetTexture(sprite.assetId),
			//	&srcRect,
			//	&dstRect,
			//	transform.rotation,
			//	NULL,
			//	sprite.flip
			//);

		}
	}
};