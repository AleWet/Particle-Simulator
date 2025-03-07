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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //core profile ==> no standar VA 

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1280, 960, "Hello World", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1); // Set swap interval to 1 to enable V-Sync -> now that I have implemented the fixed timeStep it's useless

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
    GLCall(glViewport(0, 0, 1280, 960));

    { //additional scope to avoid memory leaks
        // Define vertex positions with position and color data
        float positions[] = {
            // Position (x,y)  Color (r,g,b)   Texture (u,v)
            -2.5f,  2.5f,      1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // Top-left
             2.5f,  2.5f,      0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // Top-right
             2.5f, -2.5f,      0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Bottom-right
            -2.5f, -2.5f,      0.5f, 0.0f, 0.5f,   0.0f, 0.0f  // Bottom-left
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Enable blending
        GLCall(glEnable(GL_BLEND));
        
        // Specify bleding function
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Create and set up the vertex array object
        VertexArray va;

        // Create and bind the vertex buffer
        VertexBuffer vb(positions, 4 * 7 * sizeof(float));

        // Create and set up the vertex buffer layout
        VertexBufferLayout layout;
        layout.Push<float>(2);  // Position:       2 floats
        layout.Push<float>(3);  // Color:          3 floats
        layout.Push<float>(2);  // Texture Coords: 2 floats

        // Add the buffer with the layout to the vertex array
        va.AddBuffer(vb, layout);

        // Create the index buffer
        IndexBuffer ib(indices, 6); 

        // Initialize translatoion for obj A
        glm::vec3 transA = glm::vec3(10.0, -10.0, 0.0);

        // initialize rotation for obj A
        glm::vec3 rotA = glm::vec3(0.0, 0.0, 1.0);

        // Initialize transformation for obj B
        glm::vec3 transB = glm::vec3(0.0, 2.0, 0.0);

        // Initialize proj matrix (left, right, bottom, top)
        glm::mat4 proj = glm::ortho(-20.0f, 20.0f, -15.0f, 15.0f);

        // Initialize view matrix 
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(5.0, 5.0, 0.0));

        // Parse and create the shader program
        Shader shader("res/shaders/Basic.shader"); 

        // default to binding the texture to slot 0
        shader.Bind();

        // the 0 represents the slot I've bound the texture to
        shader.setUniform1i("u_Texture", 0); 
              
        // create texture 
        Texture texture("res/textures/obsidianLogoNoBg.png"); 

        // default 0
        texture.Bind(); 

        // Unbind everything initially
        shader.UnBind();
        va.UnBind();
        vb.UnBind();
        ib.UnBind();

        // Create Renderer
        Renderer renderer;
        
        float totalAngle = 0;
        float w = 10.00f; // speed of rotation

        Time timeManager;
        int counter = 0;

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            // Set clear color and clear the buffer
            renderer.Clear();

            int steps = timeManager.update();
            float fixedTime = timeManager.getFixedDeltaTime();

            
            if (counter++ == 1000)
            {
                std::cout << timeManager.getLastfps() << std::endl;
                counter = 0;
            }

            for (int i = 0; i < steps; i++)
            {
                // update physics simulation according to number of steps
                totalAngle += w * fixedTime;
            }


            { // Object A
                // Start with identity matrix
                glm::mat4 model = glm::mat4(1.0f);

                // Apply translation
                model = glm::translate(model, transA);

                // Apply rotation (angle in radians, rotation axis) --> Rotate 45 degrees around rotA point
                // this applies the rotation around the object, not around the point (0, 0) of the geometry coordinates
                model = glm::rotate(model, glm::radians(totalAngle), rotA);

                // Compute the final MVP matrix
                glm::mat4 mvp = proj * view * model;
                
                //Bind shader(this is here just for logic)
                shader.Bind();

                // Create uniform for mpv
                shader.setUniformMat4f("u_MVP", mvp);

                // draw call for obj A
                renderer.Draw(va, ib, shader);
            }

            { // Object A
                // translate model matrix B
                glm::mat4 model = glm::translate(glm::mat4(1.0f), transB);
                
                // Initialize MVP B
                glm::mat4 mvp = proj * view * model;

                // Bind shader (this is here just for logic)
                shader.Bind();

                // Create uniform for mpv
                shader.setUniformMat4f("u_MVP", mvp);

                // draw call for obj B
                renderer.Draw(va, ib, shader);
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