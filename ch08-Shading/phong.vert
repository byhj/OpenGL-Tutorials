#version 430 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;

out VS_OUT
{
  vec3 L;
  vec3 N;
  vec3 V;
  vec2 tc;
}vs_out;

uniform mat4 mv;
uniform mat4 proj;

uniform vec3 lightPos = vec3(1.0, 1.0, -1.0);

void main()
{
    vec4 P = mv * vec4(Position, 1.0);	   

	vs_out.N = mat3(mv) * Normal;
	vs_out.L = lightPos - P.xyz;
	vs_out.V = -P.xyz;
	vs_out.tc = TexCoords;
    gl_Position = proj * P;
}