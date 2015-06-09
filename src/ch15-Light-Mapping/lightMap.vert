#version 330 core


layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

out vec2 UV;
uniform mat4 MVP;

void main()
{

	UV = TexCoord;
	gl_Position =  MVP * vec4(Position, 1.0f);
}

