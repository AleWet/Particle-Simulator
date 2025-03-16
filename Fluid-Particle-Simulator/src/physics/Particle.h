#pragma once

#include "glm/glm.hpp"
#include "Vec2.h"

struct Particle
{
	Vec2 position;
	Vec2 velocity;
	Vec2 force; // the same as acceleration
	float mass;

	float temperature;
	float density;   // ?
	float pressure;  // ?

	Particle(const Vec2& pos, const Vec2& vel, float m = 1.0f)
		: position(pos), velocity(vel), force({0.0, 0.0}),
		density(0.0f), pressure(0.0f), mass(m), temperature(20.0f)
	{
	}
};