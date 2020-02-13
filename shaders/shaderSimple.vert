#version 400 core
layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 color;
//layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 pos = vec3(position.x*10.f, position.y*0.05f, position.z*10.f);
	gl_Position =  projection * view * model * vec4(pos, 1.0f);
}
