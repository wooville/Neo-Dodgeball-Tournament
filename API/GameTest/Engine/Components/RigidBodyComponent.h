#pragma once

struct RigidBodyComponent {
	float velocityX;
	float velocityY;

	RigidBodyComponent(float velocityX = 0, float velocityY = 0) {
		this->velocityX = velocityX;
		this->velocityY = velocityY;
	}
};
