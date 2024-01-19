#pragma once

struct BoxColliderComponent {
public:
	int width;
	int height;
	int offsetX;
	int offsetY;

	BoxColliderComponent(int width = 0, int height = 0, int offsetX = 0, int offsetY = 0) {
		this->width = width;
		this->height = height;
		this->offsetX = offsetX;
		this->offsetY = offsetY;
	}
};