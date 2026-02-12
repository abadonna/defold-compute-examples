#version 140

out vec4 frag_color;
in vec2 var_tex;

uniform sampler2D tex0;

uniform fs_uniforms
{
	highp mat4 invproj;
	highp vec4 texel;
};

vec4 view_pos(vec2 uv) {
	float depth = texture(tex0, uv).x;
	vec4 clip_pos = vec4(uv * 2.0 - 1.0, 1.0, 1.0);
	vec4 view_ray = invproj * clip_pos;
	view_ray /= view_ray.w; // Perspective divide
	return vec4(view_ray.xyz * (-depth / view_ray.z), depth);
}

void main()
{
	vec4 center = view_pos(var_tex);

	if (center.w < .001) 
	{
		discard;
	}

	vec3 ddx = view_pos(var_tex + vec2(texel.x, 0)).xyz - center.xyz;
	vec3 ddx2 = center.xyz - view_pos(var_tex + vec2(-texel.x, 0)).xyz;
	if (abs(ddx2.z) < abs(ddx.z))
	{
		ddx = ddx2;
	}

	vec3 ddy = view_pos(var_tex + vec2(0, texel.y)).xyz - center.xyz;
	vec3 ddy2 = center.xyz - view_pos(var_tex + vec2(0,-texel.y)).xyz;
	if (abs(ddy2.z) < abs(ddy.z)) {
		ddy = ddy2;
	}

	vec3 N = normalize(cross(ddx, ddy));

	frag_color = vec4(N, 1);
}
