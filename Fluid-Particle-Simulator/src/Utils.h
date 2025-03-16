#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "glm/gtc/matrix_transform.hpp"
#include "physics/Vec2.h"
#include "glm/glm.hpp"

// Returns true if shaderPath is valid
bool IsShaderPathOk(std::string shaderPath);

// Renders the edges of the simulations
// IN THE FUTURE I WILL MAEK GLM::VEC4 CONSINSTENT WITH ALL THE REST
void BoundsRenderer(Vec2 bottomLeft, Vec2 topRight, float borderWidth,
    glm::vec4 color, const glm::mat4& simulationViewMatrix);