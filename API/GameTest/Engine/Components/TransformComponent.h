#pragma once

struct TransformComponent {
	float x;
	float y;
	float scale;
	float rotation;

	TransformComponent(float x = 0, float y = 0, float scale = 1.0, float rotation = 0.0) {
		this->x = x;
		this->y = y;
		this->scale = scale;
		this->rotation = rotation;
	}
};