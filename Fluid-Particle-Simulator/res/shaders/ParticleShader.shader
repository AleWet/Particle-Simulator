#shader vertex
#version 330 core

// Quad vertex attributes
layout(location = 0) in vec2 a_Position;    // Quad vertex positions
layout(location = 1) in vec2 a_TexCoord;    // Texture coordinates

// Instance attributes
layout(location = 2) in vec2 a_ParticlePos; // Particle center position
layout(location = 3) in vec4 a_Color;       // Particle color
layout(location = 4) in float a_Size;       // Particle size

// Outputs to fragment shader
out vec2 v_TexCoord;
out vec4 v_Color;

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
    
    // Pass color to fragment shader
    v_Color = a_Color;
}

#shader fragment
#version 330 core

in vec2 v_TexCoord;
in vec4 v_Color;
out vec4 FragColor;

void main()
{
    // Calculate distance from center (0.5, 0.5) in texture space
    vec2 center = vec2(0.5, 0.5);
    float distance = length(v_TexCoord - center) * 2.0; // *2 to normalize to [0,1] range
    
    // Create a soft circle shape with smooth edges
    float circleShape = 1.0 - smoothstep(0.9, 1.0, distance);
    
    // Apply the circle shape to the alpha channel
    vec4 finalColor = v_Color;
    finalColor.a = finalColor.a * circleShape;
    
    // Discard pixels outside the circle to create a clean edge
    if (circleShape < 0.1) discard;
    
    FragColor = finalColor;
}