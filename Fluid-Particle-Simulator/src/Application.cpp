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
#include "ParticleRenderer.h"

#include "physics/SimulationSystem.h"
#include "physics/Physics.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Add this function to your Application.cpp or create a separate file
void RenderSimulationFrame(const SimulationSystem& sim, const glm::mat4& viewMatrix)
{
    // Create a simple shader for rendering particles
    static GLuint shaderProgram = 0;
    static GLint mvpLocation = -1;

    // Initialize the shader if it doesn't exist
    if (shaderProgram == 0) {
        // Create an inline shader since we don't want to depend on external files
        const char* vertexShaderSource = R"(
            #version 330 core
            layout(location = 0) in vec2 position;
            
            uniform mat4 u_MVP;
            
            void main()
            {
                gl_Position = u_MVP * vec4(position, 0.0, 1.0);
                gl_PointSize = 5.0; // Fixed point size
            }
        )";

        const char* fragmentShaderSource = R"(
            #version 330 core
            out vec4 color;
            
            void main()
            {
                // Simple circle shape with soft edges
                vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
                float circleShape = 1.0 - smoothstep(0.0, 1.0, length(circCoord));
                
                // Blue particles
                color = vec4(0.2, 0.4, 0.8, circleShape);
                if (circleShape < 0.1) discard; // Discard pixels outside the circle
            }
        )";

        // Create and compile vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLCall(glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr));
        GLCall(glCompileShader(vertexShader));

        // Check vertex shader compilation
        GLint success;
        GLCall(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
        if (!success) {
            char infoLog[512];
            GLCall(glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog));
            std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
            return;
        }

        // Create and compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        GLCall(glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr));
        GLCall(glCompileShader(fragmentShader));

        // Check fragment shader compilation
        GLCall(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));
        if (!success) {
            char infoLog[512];
            GLCall(glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog));
            std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
            return;
        }

        // Create and link shader program
        shaderProgram = glCreateProgram();
        GLCall(glAttachShader(shaderProgram, vertexShader));
        GLCall(glAttachShader(shaderProgram, fragmentShader));
        GLCall(glLinkProgram(shaderProgram));

        // Check program linking
        GLCall(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success));
        if (!success) {
            char infoLog[512];
            GLCall(glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog));
            std::cerr << "Shader program linking failed: " << infoLog << std::endl;
            return;
        }

        // Delete the shaders as they're linked into our program now and no longer necessary
        GLCall(glDeleteShader(vertexShader));
        GLCall(glDeleteShader(fragmentShader));

        // Get the location of the MVP uniform
        mvpLocation = glGetUniformLocation(shaderProgram, "u_MVP");
        if (mvpLocation == -1) {
            std::cerr << "Warning: Couldn't find uniform 'u_MVP' in shader program" << std::endl;
        }
    }

    // Get particles from simulation
    const std::vector<Particle>& particles = sim.GetParticles();

    // Create a simple vertex buffer with just positions
    std::vector<float> vertexData;
    vertexData.reserve(particles.size() * 2); // 2 floats per particle (x, y)

    for (const auto& p : particles) {
        vertexData.push_back(p.position.x);
        vertexData.push_back(p.position.y);
    }

    // Create and bind a vertex array
    GLuint vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    // Create and bind a vertex buffer
    GLuint vbo;
    GLCall(glGenBuffers(1, &vbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW));

    // Define vertex attributes
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

    // Clear the screen
    GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f)); // Dark background
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    // Use shader program directly
    GLCall(glUseProgram(shaderProgram));

    // Set the MVP uniform
    GLCall(glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &viewMatrix[0][0]));

    // Draw particles as points
    GLCall(glEnable(GL_PROGRAM_POINT_SIZE)); // Enable point size in shader
    GLCall(glDrawArrays(GL_POINTS, 0, particles.size()));

    // Cleanup
    GLCall(glDisable(GL_PROGRAM_POINT_SIZE));
    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glUseProgram(0));

    // Delete temporary resources
    GLCall(glDeleteBuffers(1, &vbo));
    GLCall(glDeleteVertexArrays(1, &vao));
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

        std::string shaderPath = "res/shaders/ParticleShader.shader";

        // First check if the shader file exists
        std::ifstream fileCheck(shaderPath);
        if (!fileCheck.good()) {
            std::cerr << "Error: Cannot open shader file: " << shaderPath << std::endl;
            // Handle the error - maybe set a flag or throw an exception
            return 0;
        }
        

        //initialzie particle renderer
        //ParticleRenderer renderer(stocazzoPath);

        // Initialize counter for fps 
        int counter = 0;

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            // update physics before rendering
            int steps = timeManager.update();
            for (int i = 0; i < steps; i++)
            {
                UpdatePhysics(sim, timeManager.getFixedDeltaTime());
            }

            //renderer.update(sim.GetParticles());

            //renderer.render(sim.GetViewMatrix(zoom, aspectRatio));

            RenderSimulationFrame(sim, sim.GetViewMatrix(zoom, aspectRatio));

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