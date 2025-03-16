#pragma once

#include "SimulationSystem.h"
#include "SolveCollision.h"


// Update particles inside simulation system particle vector in fixed deltaTime
void UpdatePhysics(SimulationSystem& sim, float deltaTime, bool useSpacePart);