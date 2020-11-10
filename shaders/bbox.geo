#version 450 core
layout (lines) in;
layout (line_strip, max_vertices = 24) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void addLine(vec4 p0, vec4 p1)
{
  gl_Position = p0;
  EmitVertex();
  gl_Position = p1;
  EmitVertex();
  EndPrimitive();
}

void main()
{
	vec4 min = gl_in[0].gl_Position; 
	vec4 max = gl_in[1].gl_Position;
	
	min.w = 1.0f;
	max.w = 1.0f;

	vec4 p0 = min;
	vec4 p1 = min + vec4(max.x*2.0f,0,0,0.0f);
	vec4 p2 = min + vec4(0,max.y*2.0f,0,0.0f);
	vec4 p3 = min + vec4(0,0,max.z*2.0f,0.0f);
	
	vec4 p4 = max;
	vec4 p5 = max + vec4(min.x*2.0f,0,0,0.0f);
	vec4 p6 = max + vec4(0,min.y*2.0f,0,0.0f);
	vec4 p7 = max + vec4(0,0,min.z*2.0f,0.0f);

	
	p0 = projection * view * model * p0;
	p1 = projection * view * model * p1;
	p2 = projection * view * model * p2;
	p3 = projection * view * model * p3;
	
	p4 = projection * view * model * p4;
	p5 = projection * view * model * p5;
	p6 = projection * view * model * p6;
	p7 = projection * view * model * p7;
	
	addLine(p0, p1);
	addLine(p0, p2);
	addLine(p0, p3);
	
	addLine(p4, p5);
	addLine(p4, p6);
	addLine(p4, p7);
	
	addLine(p3, p5);
	addLine(p3, p6);
	addLine(p2, p5);
	
	addLine(p1, p7);
	addLine(p7, p2);
	addLine(p6, p1);

}

