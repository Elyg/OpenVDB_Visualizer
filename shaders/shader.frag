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
	float StepSize = 0.01;
	vec3 dir = exitPoint - EntryPoint;
	
	dir = normalize(dir);
	if (EntryPoint == exitPoint)
		//background need no raycasting
		//FragColor = vec4(0.0f,0.0f,0.0f,0.0f);
		discard;
	float density = 0;
	float val = 0;
    //dst = (1.0 - dst.a) * src + dst;
	vec3 voxelCoord = EntryPoint;

	for(int i = 0; i < 600; i++)
	{
		val = texture(volume, voxelCoord).x;

		density = (1.0f - density)*(val*StepSize)+ density;
		
		voxelCoord += (dir*StepSize);
		
	}
	
	float alpha = density;
	FragColor = vec4(1.0f*alpha, 1.0f*alpha, 1.0f*alpha, alpha);
	//FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	//FragColor = vec4(exitPoint, 1.0f);
}
//vec3 directLight(vec3 pos){
    
//    vec3 absorption = vec3(1.0);
    
//    for(int i = 0; i < MaxSteps; i++){
//        float dist = distanceEstimation(pos);
//        pos -= LightDir * max(dist, StepSize);
//        if(dist < StepSize) {
//            float abStep = StepSize * randomFloat();
//            pos -= LightDir * (abStep-StepSize);
//            if(dist < 0.0){
//                float absorbance = exp(-Density*abStep);
//                absorption *= absorbance;
//                if(maxV(absorption) < 1.0-MaxShadowAbso) break;
//            }
//        }
        
//        if(length(pos) > SceneRadius) break;
//    }
//    return LightColor * max((absorption+MaxShadowAbso-1.0) / MaxShadowAbso, vec3(0));
//}

//vec3 pathTrace(vec3 rayPos, vec3 rayDir){
    
//   	rayPos += rayDir * max(length(rayPos)-SceneRadius, 0.0);
    
//    vec3 outColor = vec3(0.0);
//    vec3 absorption = vec3(1.0);
    
//    for(int i = 0; i < MaxSteps; i++){
//        float dist = distanceEstimation(rayPos);
//        rayPos += rayDir * max(dist, StepSize);
//        if(dist < StepSize && length(rayPos) < SceneRadius) {
//            float abStep = StepSize * randomFloat();
//            rayPos += rayDir * (abStep-StepSize);
//            if(dist < 0.0){
                
//                float absorbance = exp(-Density*abStep);
//                float transmittance = 1.0-absorbance;
                
//                //surface glow for a nice additional effect
//                //if(dist > -.0001) outColor += absorption * vec3(.2, .2, .2);
                
//                if(randomFloat() < ShadowRaysPerStep) outColor += 1.0/ShadowRaysPerStep * absorption * transmittance * VolumeColor * directLight(rayPos);
//                if(maxV(absorption) < 1.0-MaxAbso) break;
//                if(randomFloat() > absorbance) {
//                    rayDir = randomDir();
//                    absorption *= VolumeColor;
//                }
//            }
//        }
        
//        if(length(rayPos) > SceneRadius && dot(rayDir, rayPos) > 0.0)
//            return outColor + backgroundColor(rayDir) * absorption;
//    }
    
//    return outColor;
//}
