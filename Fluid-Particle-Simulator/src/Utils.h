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

#include "glm/glm.hpp"

// Returns true if shaderPath is valid
bool IsShaderPathOk(std::string shaderPath);

// Renders the edges of the simulations
void BoundsRenderer(glm::vec2 bottomLeft, glm::vec2 topRight, float borderWidth,
    glm::vec4 color, float simulationBorderOffset,
    const glm::mat4& simulationViewMatrix);