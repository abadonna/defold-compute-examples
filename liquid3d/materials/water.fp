#version 140

out vec4 frag_color;
in vec2 var_tex;

uniform sampler2D tex_data; //smooth depth, smooth thickness, 0, depth
uniform sampler2D tex_normals;
uniform sampler2D tex_color; //scene color
uniform sampler2D tex_depth; //scene depth
uniform samplerCube tex_env; //environment 

uniform fs_uniforms
{
	mediump vec4 tint;
	highp mat4 invproj;
	highp mat4 invview;
};


float get_view_z(float hw_depth) {
	float z_near = 0.1;
	float z_far  = 1000;
	// Map 0..1 to NDC -1..1
	float ndc_z = hw_depth * 2.0 - 1.0;
	// Inverse Projection Z
	return (2.0 * z_near * z_far) / (z_far + z_near - ndc_z * (z_far - z_near));
}

vec3 view_pos(float depth) {
	vec4 clip_pos = vec4(var_tex * 2.0 - 1.0, 1.0, 1.0);
	vec4 view_ray = invproj * clip_pos;
	view_ray /= view_ray.w; // Perspective divide
	return view_ray.xyz * (-depth / view_ray.z);
}

void main()
{
	vec4 data = texture(tex_data, var_tex);
	
	float depth = data.x;
	float thickness = data.y;

	if (depth < .001) {
		frag_color = texture(tex_color, var_tex);
		return;
	}

	float check = get_view_z(texture(tex_depth, var_tex).x) + 0.01;
	
	if (check < depth) {
		frag_color = texture(tex_color, var_tex);
		return;
	}

	vec3 normal = texture(tex_normals, var_tex).xyz;
	
	vec3 light = normalize(vec3(0.5, 1.0, 0.5));
	//float shading = dot(normal, light) * .5 + .5;
	//frag_color = vec4(shading, shading, shading, 1);

	vec2 uv = var_tex + (normal.xy * 0.02);
	vec3 background = texture(tex_color, uv).rgb;

	vec3 view_dir = normalize(-view_pos(depth)); // eye in view space is at 0,0,0 looking down -Z
	vec3 h = normalize(light + view_dir);

	float NdotH = max(dot(normal, h), 0.0);
	float specular = pow(NdotH, 64.0);

	float NdotV = max(dot(normal, view_dir), 0.0);
	float fresnel = 0.1 + (1.0 - 0.1) * pow(1.0 - NdotV,4.0);

	vec3 r = mat3(invview) * reflect(-view_dir, normal);
	vec3 sky = texture(tex_env, r).xyz * 2.;

	float density = 0.7;
	vec3 transmission = exp(-thickness * density * (1.0 - tint.rgb));

	vec3 color = mix(background * transmission, sky, fresnel) + specular;
	
	frag_color = vec4(mix(background, color, smoothstep(0, .7, thickness)), 1);
	
}
