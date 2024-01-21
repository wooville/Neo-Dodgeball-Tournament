#pragma once

struct RigidBodyComponent {
	float velocityX;
	float velocityY;
	//bool isKinematic;
	float acceleration;

	RigidBodyComponent(float velocityX = 0.0f, float velocityY = 0.0f, /*bool isKinematic = true, */float acceleration = 1.0f) {
		this->velocityX = velocityX;
		this->velocityY = velocityY;
		//this->isKinematic = isKinematic;
		this->acceleration = acceleration;
	}
};
