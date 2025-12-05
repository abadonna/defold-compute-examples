#version 140

out vec4 frag_color;
in vec2 var_tex;
uniform sampler2D tex0;

void main()
{
	vec4 color = texture(tex0, var_tex);
	frag_color = color;
}
