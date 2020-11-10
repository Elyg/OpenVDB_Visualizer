#version 450 core
out vec4 FragColor;

uniform sampler3D volume;
uniform sampler1D transfer;
in vec3 pos;
uniform vec3 color;
uniform float slices;
uniform float maxDim;

uniform float min;
uniform float max;
uniform float densityMulti;

float fit(float value, float min1, float max1, float min2, float max2) 
{
  float val = min2 + (value - min1) * (max2 - min2) / (max1 - min1);
  return clamp(val, min2, max2);
}

void main()
{
	float density = texture(volume, pos).r;
	float val = fit(density, min, max, 0, 1);
	vec3 rampColor = texture(transfer, val).xyz;
	
	float alpha = clamp(val,0, 1);
	alpha *= densityMulti;
	alpha *= maxDim/float(slices);
	
	FragColor = vec4(rampColor, alpha);

}

