#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float size;

out vec4 v_Color;

uniform mat4 u_MVP;
uniform float u_PointSizeScale;

void main()
{
    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
    gl_PointSize = size * u_PointSizeScale;
    v_Color = color;
}

#shader fragment
#version 330 core
in vec4 v_Color;
out vec4 FragColor;

void main()
{
    // Calculate distance from center of point (in the range 0.0 to 1.0)
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float distance = length(circCoord);
    
    // Create a soft circle shape with smooth edges
    float circleShape = 1.0 - smoothstep(0.99, 1.0, distance);
    
    // Apply the circle shape to the alpha channel
    vec4 finalColor = v_Color;
    finalColor.a = finalColor.a * circleShape;
    
    // Discard pixels outside the circle to create a clean edge
    if (circleShape < 0.1) discard;
    
    FragColor = finalColor;
}