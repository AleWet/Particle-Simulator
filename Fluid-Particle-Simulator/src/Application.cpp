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
         


        // Set up simulation boundaries based on screen coordinates
        // I'll use normalized device coordinates for simplicity, then scale with view matrix
        float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

        // Arbitrary world units for simulation width
        float simWidth = 200.0f;  

        // Particle size (in simulation units) ==> this size is relative to the simulation units
        // there is a bug with this because this is not the actual rendered size, this is just for the physics at the moment
        unsigned int particleRadius = 5.0f;
        
        // Make simulation rectangle the same ratio of the screen for simplicity
        float simHeight = simWidth / aspectRatio; 

        // Define simulation boundaries centered on the origin
        glm::vec2 bottomLeft(-simWidth / 2, -simHeight / 2);
        glm::vec2 topRight(simWidth / 2, simHeight / 2);

        // Distance from simulation border and window
        float simulationBorderOffset = 10.0f;

        // Set zoom
        float zoom = 0.5f;

        // Add particles in a grid pattern 
        int rows = 3;
        int cols = 3;
        
        // Set particle grid coordinates, this creates a rectangle that will be moved to the center of the simulation,
        // At the moment the rectangle is centered in the origin ==> it will also be in the center of the simulation
        glm::vec2 gridBottomLeft(-60.0f, -60.0f);  // Smaller than full simulation bounds
        glm::vec2 gridTopRight(60.0f, 60.0f);

        // Space between particles
        glm::vec2 spacing(0.0f, 0.0f);           
        float particleMass = 1.0f;

        // Set border rendering parameters
        glm::vec4 simBorderColor(1.0f, 1.0f, 1.0f, 0.5f); // White
        glm::vec4 gridBorderColor(0.0f, 1.0f, 0.0f, 0.5f); // Green
        float borderWidth = 2.0f;



        // --------------------------------------------------------------------------------


        // Create simulation system
        SimulationSystem sim(bottomLeft, topRight, particleRadius, WINDOW_WIDTH);

        sim.AddParticleGrid(rows, cols, gridBottomLeft, gridTopRight, spacing, particleMass);
       
        // Enable blending
        GLCall(glEnable(GL_BLEND));
        
        // Specify bleding function
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Initialize shader path 
        std::string shaderPath = "res/shaders/ParticleShader.shader";

        // First check if the shader file exists
        if (!IsShaderPathOk(shaderPath)) 
            return 0;

        // initialize shader outside of renderer (easier and more flexible)
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
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));  // Dark background

            // Set zoom for simulation
            sim.SetZoom(zoom);

            // create viewMatrix after setting the zoom
            glm::mat4 viewMatrix = sim.GetViewMatrix(simulationBorderOffset);


            // Update physics before rendering
            int steps = timeManager.update();
            for (int i = 0; i < steps; i++)
                UpdatePhysics(sim, timeManager.getFixedDeltaTime());

            // Update buffers with new particle data
            renderer.UpdateBuffers();

            // Render the particles with the view matrix
            renderer.Render(sim, viewMatrix);

            // Render simulation borders
            BoundsRenderer(sim.GetBounds()[0], sim.GetBounds()[1], borderWidth, simBorderColor, simulationBorderOffset, viewMatrix);
            BoundsRenderer(gridBottomLeft, gridTopRight, borderWidth, gridBorderColor, 0, viewMatrix);

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
