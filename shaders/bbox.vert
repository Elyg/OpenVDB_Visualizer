#version 450 core
layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 color;
//layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos;

void main()
{
	//gl_LineWidth = 2.0f;
	gl_Position =  vec4(position, 1.0f);
}
