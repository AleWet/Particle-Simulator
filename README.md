# Particle Simulator With Euler Integration Method

## Overview
This project is a physics-based particle simulator written in C++ using OpenGL. It was designed for physics testing and employs Euler's integration along with a naive space partitioning algorithm for simulation. The project is currently outdated, but contributions are welcome.

## Features
- **Euler Integration** for physics calculations
- **Space Partitioning** for performance optimization
- **Customizable Simulation Parameters** (set before compilation)
- **GLFW & GLEW for OpenGL rendering**
- **GLM for mathematical computations** (and custom math library)

## Dependencies
The project requires the following libraries, all included in the `Dependencies` folder:
- [GLFW 3.4](https://www.glfw.org/)
- [GLEW 2.1.0](http://glew.sourceforge.net/)
- [GLM (Mathematics Library)](https://glm.g-truc.net/0.9.9/index.html)

## Installation & Setup
### 1. Configure Project Properties
Before running the simulator, ensure the following settings are correctly configured in your development environment:
- **Include Directories:**
  - `$(SolutionDir)Dependencies\glfw-3.4.bin.WIN32\include`
  - `$(SolutionDir)Dependencies\glew-2.1.0\include`
  - `src\vendor`
- **Library Directories:**
  - `$(SolutionDir)Dependencies\glfw-3.4.bin.WIN32\lib-vc2022`
  - `$(SolutionDir)Dependencies\glew-2.1.0\lib\Release\Win32`
- **Additional Dependencies:** 
  - `glfw3.lib`
  - `glew32s.lib`
  - `opengl32.lib`
  - `user32.lib`
  - `gdi32.lib`
  - `shell32.lib`

### 2. Build and Run
1. Open the project in **Visual Studio** or your preferred C++ IDE.
2. Ensure the dependencies are correctly linked as shown above.
3. Build and run the project.

## Usage
Simulation parameters must be set **before compilation** within the `application.cpp` file under **SIMULATION PARAMETERS**:
```cpp
// Set zoom
float zoom = 0.7f;

// Define the number of particles in a grid pattern
int rows = 20;
int cols = 20;

// Particle properties
float particleRadius = 20.0f;
float particleMass = 1.0f;
bool withInitialVelocity = true;

(...)
```
The parameters cannot be modified at runtime. Modify them in the source code and recompile to apply changes.

## Known Issues & Limitations
- **Performance Limit:** The simulation struggles with more than **3000 particles** (as of the 16/03/2025) with 6 substeps due to performance constraints.

## Contribution
This project is open for contributions. Feel free to submit pull requests to improve performance, fix issues, or add features.
