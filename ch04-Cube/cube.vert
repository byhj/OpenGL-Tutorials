#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

uniform mat4 mvp;

out vec3 vColor;

void main(void)
{
   vColor = Color;
   gl_Position = mvp * vec4(Position, 1.0);
}