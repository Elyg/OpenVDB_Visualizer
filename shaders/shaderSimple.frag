#version 450 core
out vec4 FragColor;

uniform sampler3D volume;
in vec3 pos;
uniform vec3 color;
uniform float slices;
void main()
{
	
	FragColor = vec4(1.0f, 0.0f, 0.0f, 0.1f);
	FragColor = vec4(color, 1.0f);
	float intensity = texture(volume, pos).r;
	float org = intensity;
	float samplingRate = 1;
	intensity *= samplingRate;
	//intensity = 1 - pow((1 - intensity), samplingRate/slices);
	intensity = clamp(intensity, 0, 1)*0.1;
	//FragColor = vec4(intensity,intensity,intensity,clamp(intensity, 0, 1));
}

