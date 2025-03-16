#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "physics/SimulationSystem.h"
#include "physics/Physics.h"

#include "Shader.h"
#include "Texture.h"
#include "core/Time.h"
#include "ParticleRenderer.h"
#include "Utils.h"

// other includes are in Utils.h


// Updates the window title with formatted performance metrics
void UpdateWindowTitle(GLFWwindow* window, const Time& timeManager, const std::string& appName = "Particle Simulation")
{
    // Format FPS with consistent width (6 chars: ####.#)
    char fpsBuffer[32];
    snprintf(fpsBuffer, sizeof(fpsBuffer), "%6.1f", timeManager.getLastfps());

    char avgFpsBuffer[32];
    snprintf(avgFpsBuffer, sizeof(avgFpsBuffer), "%6.1f", timeManager.getAverageFPS());

    // Format MSPF with consistent width (6 chars: ###.##)
    char mspfBuffer[32];
    snprintf(mspfBuffer, sizeof(mspfBuffer), "%6.2f", timeManager.getLastFrameTimeMs());

    char avgMspfBuffer[32];
    snprintf(avgMspfBuffer, sizeof(avgMspfBuffer), "%6.2f", timeManager.getAverageFrameTimeMs());

    // Combine into a nicely formatted title with fixed width fields
    std::string title = appName + " | " +
        "FPS: " + fpsBuffer + " (Avg: " + avgFpsBuffer + ") | " +
        "MS: " + mspfBuffer + " (Avg: " + avgMspfBuffer + ")";

    // Use fixed-width status indicators
    float targetFPS = 60.0f;
    float avgFPS = timeManager.getAverageFPS();

    if (avgFPS >= targetFPS * 0.95f) {
        title += " [Good]    "; // Fixed width padding
    }
    else if (avgFPS >= targetFPS * 0.8f) {
        title += " [Average] "; // Same width as other indicators
    }
    else {
        title += " [Poor]    "; // Same width as other indicators
    }

    // Update the window title
    glfwSetWindowTitle(window, title.c_str());
}


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


        // ================== SIMULATION PARAMETERS ==================
         
        // --------- GENERAL --------- 
            
        // Set up simulation boundaries based on screen coordinates
        // I'll use normalized device coordinates for simplicity, then scale with view matrix
        const float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

        // Arbitrary world units for simulation width
        const float simWidth = 2000.0f;
        
        // Make simulation rectangle the same ratio of the screen for simplicity
        const float simHeight = simWidth / aspectRatio; 

        // Set zoom
        const float zoom = 0.7f;

        // Toggle space partitioning
        bool useSpacePartitioning = true;

        // Number of substeps for simulation
        const unsigned int subSteps = 1;

        // Particle size (in simulation units)
        const float particleRadius = 4.0f;

        // --------- PARTICLE CREATION --------- 
              
        // --- GRID ---
                
        int rows = 82;
        int cols = 85;
        const Vec2 spacing = { 0.0, 0.0 };
        const float particleMassGrid = 1.0f;
        const bool withInitialVelocityGrid = true;

        // --- STREAM ---

        const unsigned int totalParticlesPerStream = 2900; // (limit with 1 substep is 9000 and without energy loss)
        const float StreamSpeed = 150.0f;                  // Particle spawn rate
        const float particleMassStream = 1.0f;
        const Vec2 initialVelocityStream0 = {100.0, -100.0};
        const Vec2 initialVelocityStream1 = { -100.0f, -100.0f };
        const Vec2 initialVelocityStream2 = { 100.0, -100.0 };
        
        // ---------  BORDER --------- 

        // Set border rendering parameters
        glm::vec4 simBorderColor(1.0f, 1.0f, 1.0f, 0.5f); // White
        glm::vec4 gridBorderColor(0.0f, 1.0f, 0.0f, 0.5f); // Green
        float borderWidth = 2.0f;

        // =========================================================



        // Define simulation boundaries centered on the origin
        Vec2 bottomLeft(-simWidth / 2, -simHeight / 2);
        Vec2 topRight(simWidth / 2, simHeight / 2);

        // Create simulation system
        SimulationSystem sim(bottomLeft, topRight, particleRadius, WINDOW_WIDTH);
       
        // Add particle streams
        sim.AddParticleStream(totalParticlesPerStream, StreamSpeed,
            initialVelocityStream0, particleMassStream,
            { 0.0, 0.0 });

        

        sim.AddParticleStream(totalParticlesPerStream, StreamSpeed,
            initialVelocityStream1,
            particleMassStream,
            { 1996.0, 0.0 });

        sim.AddParticleStream(totalParticlesPerStream, StreamSpeed,
            initialVelocityStream2,
            particleMassStream,
            { 1000.0, 0.0 });

        // Enable blending
        GLCall(glEnable(GL_BLEND));
        
        // Specify bleding function
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Initialize shader path 
        std::string shaderPath = "res/shaders/ParticleShader.shader";

        // First check if the shader file exists
        // this in the future will be inside the shader (maybe)
        if (!IsShaderPathOk(shaderPath)) 
            return 0;

        // initialize shader outside of renderer (easier but not permanent)
        Shader shader(shaderPath);

        // initialize particle renderer
        ParticleRenderer renderer(sim, shader);

        // Create time manager
        Time timeManager(1.0f / 60.0f);

        // Initialize counter for fps 
        int counter = 0;

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            // Clear the screen
            GLCall(glClear(GL_COLOR_BUFFER_BIT));
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));  // Black background

            // Set zoom for simulation
            sim.SetZoom(zoom);

            // Setup border mvp, this in the future will be inside 
            // particle renderer of sim system (maybe)
            glm::mat4 borderMVP = sim.GetProjMatrix() * sim.GetViewMatrix();

            // Update physics before rendering
            int steps = timeManager.update();
            for (int i = 0; i < steps; i++)
            {
                for (int j = 0; j < subSteps; j++)
                {
                    UpdatePhysics(sim, timeManager.getFixedDeltaTime() / subSteps, useSpacePartitioning);
                }
            }

            // Update buffers with new particle data
            renderer.UpdateBuffers();

            // Render the particles 
            renderer.Render();

            // Render simulation borders
            // This implementation isn't the best but good enough
            const auto& bounds = sim.GetBounds();

            BoundsRenderer(bounds.bottomLeft, bounds.topRight, borderWidth, simBorderColor, borderMVP);

            // Display fps and mspf
            if (++counter > 75)
            {
                UpdateWindowTitle(window, timeManager);
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
