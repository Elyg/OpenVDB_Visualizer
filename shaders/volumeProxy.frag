#version 450 core
out vec4 FragColor;

uniform sampler3D volume;
uniform sampler1D transfer;
in vec3 pos;

uniform float min;
uniform float max;

uniform vec3 color;

float fit(float value, float min1, float max1, float min2, float max2) 
{
  float val = min2 + (value - min1) * (max2 - min2) / (max1 - min1);
  return clamp(val, min2, max2);
}

void main()
{
	float val = fit(pos.y, min, max, 0, 1);
	vec4 c = texture(transfer, val);
	FragColor = c;
}

