#shader vertex
#version 330 core

// Quad vertex attributes
layout(location = 0) in vec2 a_Position;    // Quad vertex positions
layout(location = 1) in vec2 a_TexCoord;    // Texture coordinates

// Instance attributes
layout(location = 2) in vec2 a_ParticlePos; // Particle center position
layout(location = 3) in vec2 a_Velocity;    // Particle velocity 
layout(location = 4) in float a_Size;       // Particle size

// Outputs to fragment shader
out vec2 v_TexCoord;
out vec2 v_Velocity;

uniform mat4 u_MVP;

void main()
{
    // Calculate the position of this vertex
    // a_Position is in [-1,1] range, scale by particle size and add to particle position
    vec2 vertexPos = a_ParticlePos + a_Position * a_Size;
    
    // Transform vertex to clip space
    gl_Position = u_MVP * vec4(vertexPos, 0.0, 1.0);
    
    // Pass texture coordinates to fragment shader
    v_TexCoord = a_TexCoord;
    
    // Pass velocity to fragment shader
    v_Velocity = a_Velocity;
}

#shader fragment
#version 330 core

in vec2 v_TexCoord;
in vec2 v_Velocity;  
out vec4 FragColor;

void main()
{
    // Calculate distance from center (0.5, 0.5) in texture space
    vec2 center = vec2(0.5, 0.5);
    float distance = length(v_TexCoord - center) * 2.0; // *2 to normalize to [0,1] range
    
    // Create a soft circle shape with smooth edges
    float circleShape = 1.0 - smoothstep(0.9, 1.0, distance);

    float speed = length(v_Velocity);
    float normalizedV = min(speed / 200.0, 1.0);

    vec3 colorRGB;
    if (normalizedV < 0.25) {
        float t = normalizedV / 0.25;
        colorRGB = vec3(0.0, t, 1.0);
    }
    else if (normalizedV < 0.5) {
        float t = (normalizedV - 0.25) / 0.25;
        colorRGB = vec3(0.0, 1.0, 1.0 - t);
    }
    else if (normalizedV < 0.75) {
        float t = (normalizedV - 0.5) / 0.25;
        colorRGB = vec3(t, 1.0, 0.0);
    }
    else {
        float t = (normalizedV - 0.75) / 0.25;
        colorRGB = vec3(1.0, 1.0 - t, 0.0);
    }
    
    // Create the final color with alpha from the circle shape
    vec4 finalColor = vec4(colorRGB, circleShape);
    
    // Discard pixels outside the circle to create a clean edge
    if (circleShape < 0.1) discard;
    
    FragColor = finalColor;
}