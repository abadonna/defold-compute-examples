#version 140

out vec4 frag_color;
in vec2 var_tex;
uniform sampler2D tex_depth;
uniform sampler2D tex_thickness;

void main()
{
	float depth = texture(tex_depth, var_tex).x;
	float thickness = texture(tex_thickness, var_tex).x;
	frag_color = vec4(depth, thickness, 0, depth);
}
