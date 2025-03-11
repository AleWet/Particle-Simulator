#pragma once

#include "glm/glm.hpp"

struct Particle
{
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 force; // the same as acceleration
	float mass;

	float temperature;
	float density;   // ?
	float pressure;  // ?

	Particle(const glm::vec2& pos, const glm::vec2& vel, float m = 1.0f)
		: position(pos), velocity(vel), force(0.0f),
		density(0.0f), pressure(0.0f), mass(m), temperature(20.0f)
	{
	}
};