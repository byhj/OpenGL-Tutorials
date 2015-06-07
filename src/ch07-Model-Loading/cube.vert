#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

uniform mat4 mvp;

out vec2 tc;

void main(void)
{
   tc = TexCoord;
   gl_Position = mvp * vec4(Position, 1.0);
}