#version 450 core
out vec4 FragColor;

uniform sampler3D volume;
uniform sampler1D transfer;
in vec3 pos;
uniform vec3 color;
uniform float slices;
uniform float slice;
uniform float maxDim;

uniform float min;
uniform float max;

float fit(float value, float min1, float max1, float min2, float max2) 
{
  float val = min2 + (value - min1) * (max2 - min2) / (max1 - min1);
  return clamp(val, min2, max2);
}

void main()
{
	
	FragColor = vec4(1.0f, 0.0f, 0.0f, 0.1f);
	FragColor = vec4(color, 1.0f);
	
	float val = fit(pos.y, min, max, 0, 1);
	vec4 c = texture(transfer, val);
	//FragColor = vec4(vec3(pos.y), 1.0f);
	//FragColor = vec4(c.r,c.y,c.b, 1.0f);
	FragColor = c;
}

