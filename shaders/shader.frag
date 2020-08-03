#version 430 core
out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler3D volume;

uniform float focal_length;
uniform float aspect_ratio;
uniform vec2 viewport_size;
uniform vec3 ray_origin;
uniform vec3 top;
uniform vec3 bottom;

struct Ray
{
  vec3 origin;
  vec3 direction;
};

struct AABB
{
  vec3 top;
  vec3 bottom;
};

vec4 colour_transfer(float intensity)
{
    vec3 high = vec3(1.0, 1.0, 1.0);
    vec3 low = vec3(0.0, 0.0, 0.0);
    float alpha = (exp(intensity) - 1.0) / (exp(1.0) - 1.0);
    return vec4(intensity * high + (1.0 - intensity) * low, alpha);
}

void ray_box_intersection(Ray ray, AABB box, out float t_0, out float t_1)
{
  vec3 direction_inv = 1.0 / ray.direction;
  vec3 t_top = direction_inv * (box.top - ray.origin);
  vec3 t_bottom = direction_inv * (box.bottom - ray.origin);
  vec3 t_min = min(t_top, t_bottom);
  vec2 t = max(t_min.xx, t_min.yz);
  t_0 = max(0.0, max(t.x, t.y));
  vec3 t_max = max(t_top, t_bottom);
  t = min(t_max.xx, t_max.yz);
  t_1 = min(t.x, t.y);
}

void main()
{

  float step_length = 0.05;
  
  vec3 ray_direction;
  ray_direction.xy = 2.0 * gl_FragCoord.xy / viewport_size - 1.0;
  ray_direction.x *= aspect_ratio;
  ray_direction.z = -focal_length;
  ray_direction = (vec4(ray_direction, 0) * view).xyz;
  float t_0, t_1;
  Ray casting_ray = Ray(ray_origin, ray_direction);
  AABB bounding_box = AABB(top, bottom);
  ray_box_intersection(casting_ray, bounding_box, t_0, t_1);
  
  vec3 ray_start = (ray_origin + ray_direction * t_0 - bottom) / (top - bottom);
  vec3 ray_stop = (ray_origin + ray_direction * t_1 - bottom) / (top - bottom);
  
  vec3 ray = ray_stop - ray_start;
  float ray_length = length(ray);
  vec3 step_vector = step_length * ray / ray_length;

  // Random jitter
  //ray_start += step_vector * texture(jitter, gl_FragCoord.xy / viewport_size).r;

  vec3 position = ray_start;
  vec4 colour = vec4(0.0);
  int count = 0;
  float maximum_intensity = 0;
  // Ray march until reaching the end of the volume, or colour saturation
  while (ray_length > 0) {

	  float intensity = texture(volume, position).r;

      if (intensity > maximum_intensity) {
          maximum_intensity = intensity;
      }

      ray_length -= step_length;
      position += step_vector;
	  count ++;
  }

  // Blend background
//  colour.rgb = colour.a * colour.rgb + (1 - colour.a) * pow(background_colour, vec3(gamma)).rgb;
//  colour.a = 1.0;

  // Gamma correction
//  a_colour.rgb = pow(colour.rgb, vec3(1.0 / gamma));
//  a_colour.a = colour.a;
  
//  float density = 0;
//  float val = 0;
//  for(int i = 0; i < 600; i++)
//  {
//	  val = texture(volume, voxelCoord).x;

//	  density = (1.0f - density)*(val*StepSize)+ density;
	  
//	  voxelCoord += (dir*StepSize);
	  
//  }
  colour = colour_transfer(maximum_intensity);
  FragColor = colour;
}
