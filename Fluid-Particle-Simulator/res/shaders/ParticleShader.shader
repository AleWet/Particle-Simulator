#pragma once

#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in float size;

out vec3 v_Color;
uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
    v_Color = color;
}

#shader fragment
#version 330 core

in vec3 v_Color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(v_Color, 1.0);
}