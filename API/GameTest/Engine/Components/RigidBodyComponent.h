#pragma once

struct RigidBodyComponent {
	float velocityX;
	float velocityY;
	bool isKinematic;
	float acceleration;

	RigidBodyComponent(float velocityX = 0, float velocityY = 0, bool isKinematic = true, float acceleration = 0) {
		this->velocityX = velocityX;
		this->velocityY = velocityY;
		this->isKinematic = isKinematic;
		this->acceleration = acceleration;
	}
};
