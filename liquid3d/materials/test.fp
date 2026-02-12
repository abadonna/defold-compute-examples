#version 140

out vec4 frag_color;
in vec2 var_tex;
uniform sampler2D tex0;

void main()
{
	vec4 color = texture(tex0, var_tex);
	//color /= 500;
	//color *= color * color;
	//frag_color = vec4(color.x, color.x, color.x, 1);
	frag_color = color;
}
