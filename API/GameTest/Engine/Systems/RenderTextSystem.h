#pragma once

#include "../ECS/ECS.h"
#include "../Components/TextComponent.h"

class RenderTextSystem : public System {
public:
	RenderTextSystem() {
		RequireComponent<TextComponent>();
	}

	void Update() {
		for (auto entity : GetSystemEntities()) {
			auto& textComponent = entity.GetComponent<TextComponent>();

			for (auto t : textComponent.textToRender) {
				App::Print(t.first.first, t.first.second, t.second.c_str());
			}
			
		}
	}
};