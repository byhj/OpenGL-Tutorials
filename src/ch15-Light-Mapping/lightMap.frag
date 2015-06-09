#version 330 core

in vec2 UV;

layout (location = 0) out vec3 fragColor;

uniform sampler2D tex;

void main()
{
	// Output color = color of the texture at the specified UV
	fragColor = texture2D(tex, UV, -2.0 ).rgb;
}