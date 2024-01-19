#pragma once

#include "../App/app.h"
#include <string>

struct SpriteComponent {
	CSimpleSprite *simpleSprite;
	std::string assetId;
	int width;
	int height;
	int zIndex;

	SpriteComponent(std::string assetId = "", int columns = 1, int rows = 1, int zIndex = 0) {
		this->simpleSprite = App::CreateSprite(assetId.c_str(), columns, rows);
		this->assetId = assetId;
		this->zIndex = zIndex;
		this->width = simpleSprite->GetWidth();
		this->height = simpleSprite->GetHeight();
	}
};