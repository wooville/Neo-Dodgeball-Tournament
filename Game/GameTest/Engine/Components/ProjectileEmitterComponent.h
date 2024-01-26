#pragma once

#include "../App/app.h"

struct ProjectileEmitterComponent {
	float velocityX;
	float velocityY;
	int repeatFreq;
	int projectileDuration;
	int hitDamage;
	bool isFriendly;
	int lastEmissionTime;

	ProjectileEmitterComponent(float velocityX = 0.0, float velocityY = 0.0, int repeatFreq = 0, int projectileDuration = 10000, int hitDamage = 10, bool isFriendly = false) {
		this->velocityX = velocityX;
		this->velocityY = velocityY;
		this->repeatFreq = repeatFreq;
		this->projectileDuration = projectileDuration;
		this->hitDamage = hitDamage;
		this->isFriendly = isFriendly;
		this->lastEmissionTime = glutGet(GLUT_ELAPSED_TIME);
	}
};