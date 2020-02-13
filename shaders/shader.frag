#version 420 core
out vec4 FragColor;

in vec3 Color;
in vec4 pos;

uniform sampler3D volume;
uniform sampler2D ExitPoints;
uniform vec2 ScreenSize;

void main()
{
	//vec4 exitFragCoord = (ExitPointCoord.xy / ExitPointCoord.w + 1.0)/2.0;
	//vec2 texc = ((pos.xy/pos.w) + 1.0f) / 2.0f;
	
	//vec3 exitPoint = texture(ExitPoints, gl_FragCoord.xy/ScreenSize).xyz;
	vec3 exitPoint = texture(ExitPoints, gl_FragCoord.xy/ScreenSize).xyz;
	//vec3 exitPoint = texture(ExitPoints, texc).xyz;
	vec3 EntryPoint = Color;
	float StepSize = 0.005;
	vec3 dir = exitPoint - EntryPoint;
	
	dir = normalize(dir);
	if (EntryPoint == exitPoint)
		//background need no raycasting
		discard;
	float density = 0;
	float val = 0;
    //dst = (1.0 - dst.a) * src + dst;
	vec3 voxelCoord = EntryPoint;

	for(int i = 0; i < 600; i++)
	{
		val = texture(volume, voxelCoord).x;

		density = (1.0f - density) *(val*StepSize)+ density;
		
		voxelCoord += (dir*StepSize);
		
	}
	
	float alpha = density;
	FragColor = vec4(1.0f*alpha, 1.0f*alpha, 1.0f*alpha, alpha);
	//FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	//FragColor = vec4(exitPoint, 1.0f);
}

