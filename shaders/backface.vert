#version 420 core
layout (location = 0) in vec3 position;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position =  projection * view * model * vec4(position, 1.0f);
	Color =  position;
}
