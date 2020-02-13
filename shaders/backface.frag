#version 420 core
out vec4 FragColor;

in vec3 Color;

void main()
{
	
	FragColor = vec4(Color, 1.0f);
	//FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}

