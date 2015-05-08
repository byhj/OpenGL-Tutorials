#version 430 core

layout (location = 0) out vec4 fragColor;

in VS_OUT
{
  vec3 L;
  vec3 N;
  vec3 V;
  vec2 tc;
}fs_in;

uniform float power = 64.0f;
uniform vec3 specular_material = vec3(0.3);
uniform sampler2D tex;

void main()
{    
    vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);

	vec3 R = reflect(-L, N);
	vec3 diffuse = max(dot(N, L), 0.0) * texture2D(tex, fs_in.tc).rgb;
	vec3 specular = pow(max(dot(R, V), 0.0), power) * specular_material;
	vec3 ambient = vec3(0.5, 0.5, 0.5) * texture2D(tex, fs_in.tc).rgb;

    // calc the all light attribute
    vec3 result = (ambient + specular + diffuse);
	fragColor = vec4(result, 1.0);
}
