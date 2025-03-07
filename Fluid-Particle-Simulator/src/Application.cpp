#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include <windows.h>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "core/Time.h"

#include "physics/SimulationSystem.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int main(void)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Window dimensions
    const unsigned int WINDOW_WIDTH = 1280;
    const unsigned int WINDOW_HEIGHT = 960;

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Particle Simulation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Print debug information
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Set viewport to match window size
    GLCall(glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

    { // Additional scope to avoid memory leaks
        // Set up simulation boundaries based on screen coordinates
        // We'll use normalized device coordinates for simplicity, then scale with view matrix
        float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
        float simWidth = 200.0f;  // Arbitrary world units for simulation width
        float simHeight = simWidth / aspectRatio;

        // Define simulation boundaries centered on the origin
        glm::vec2 bottomLeft(-simWidth / 2, -simHeight / 2);
        glm::vec2 topRight(simWidth / 2, simHeight / 2);

        // Particle size (in simulation units)
        unsigned int particleRadius = 0.2f;

        // Create simulation system
        SimulationSystem sim(bottomLeft, topRight, particleRadius);

        // Create time manager
        Time timeManager(1.0f / 60.0f);

        // set fixed view matrix (for the moment)
        float zoom = 1.0f;
        glm::mat4 viewMatrix = sim.GetViewMatrix(zoom, aspectRatio);

        // Add particles in a grid pattern
        int rows = 20;
        int cols = 20;
        glm::vec2 gridBottomLeft(-8.0f, -6.0f);  // Smaller than full simulation bounds
        glm::vec2 gridTopRight(8.0f, 6.0f);
        glm::vec2 spacing(0.5f, 0.5f);  // Space between particles
        float particleMass = 1.0f;

        sim.AddParticleGrid(rows, cols, gridBottomLeft, gridTopRight, spacing, particleMass);
       
        // Enable blending
        GLCall(glEnable(GL_BLEND));
        
        // Specify bleding function
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Initialize counter for fps 
        int counter = 0;

        // Main loop
        while (!glfwWindowShouldClose(window))
        {

            int steps = timeManager.update();
            for (int i = 0; i < steps; i++)
            {
                //updateSimulationPhyisics();
            }

            // Display FPS
            if (++counter > 1000)
            {
                std::string title = "Particle Simulation - FPS: " +
                    std::to_string(static_cast<int>(timeManager.getLastfps()));
                glfwSetWindowTitle(window, title.c_str());
                counter = 0;
            }

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
        }
    }

    // Cleanup
    // I don't need to unbind any of the objects because when 
    // I reach the end of the scope that I put at the beginning 
    // every object will call its destructor and be automatically deleted
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;   
}