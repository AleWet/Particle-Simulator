#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "physics/SimulationSystem.h"
#include "physics/Physics.h"

#include "Shader.h"
#include "Texture.h"
#include "core/Time.h"
#include "ParticleRenderer.h"
#include "Renderer.h" // not needed
#include "Utils.h"

// other includes are in Utils.h


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

        // SIMULATION PARAMETERS ----------------------------------------------------------
         
        // // GENERAL 
            
        // Set up simulation boundaries based on screen coordinates
        // I'll use normalized device coordinates for simplicity, then scale with view matrix
        float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

        // Arbitrary world units for simulation width
        float simWidth = 2000.0f;  

        // Particle size (in simulation units) ==> this size is relative to the simulation units
        float particleRadius = 40.0f;
        
        // Make simulation rectangle the same ratio of the screen for simplicity
        float simHeight = simWidth / aspectRatio; 

        // Define simulation boundaries centered on the origin
        glm::vec2 bottomLeft(-simWidth / 2, -simHeight / 2);
        glm::vec2 topRight(simWidth / 2, simHeight / 2);

        // Distance from simulation border and window
        float simulationBorderOffset = 1.0f;

        // Set zoom
        float zoom = 0.7f;

        // Add particles in a grid pattern 
        int rows = 10;
        int cols = 9;

        // // PARTICLE CREATION
                  
        glm::vec2 spacing = glm::vec2(0.0f, 0.0f);
        float particleMass = 1.0f;
        bool withInitialVelocity = true;

        // // BORDER 

        // Set border rendering parameters
        glm::vec4 simBorderColor(1.0f, 1.0f, 1.0f, 0.5f); // White
        glm::vec4 gridBorderColor(0.0f, 1.0f, 0.0f, 0.5f); // Green
        float borderWidth = 2.0f;



        // --------------------------------------------------------------------------------


        // Create simulation system
        SimulationSystem sim(bottomLeft, topRight, particleRadius, WINDOW_WIDTH);

        //sim.AddParticleGrid(rows, cols, gridBottomLeft, gridTopRight, spacing, particleMass);
        sim.AddParticleGrid(rows, cols, spacing, withInitialVelocity, particleMass);
       
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

        // Bind shader
        shader.Bind();

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

            glm::mat4 MVP = sim.GetProjMatrix() * sim.GetViewMatrix();

            // Update physics before rendering
            int steps = timeManager.update();
            for (int i = 0; i < steps; i++)
                UpdatePhysics(sim, timeManager.getFixedDeltaTime());

            // Update buffers with new particle data
            renderer.UpdateBuffers();

            // Render the particles 
            renderer.Render();

            // Render simulation borders
            // This implementation isn't the best, this should be inside 
            // the renderer object or the simulation system
            BoundsRenderer(sim.GetBounds()[0], sim.GetBounds()[1], borderWidth, simBorderColor, MVP);

            // Display FPS
            if (++counter > 100)
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
