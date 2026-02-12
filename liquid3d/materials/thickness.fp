#version 140
in mediump vec2 var_texcoord0;
in mediump vec3 var_view_center; 
in mediump float var_radius;

out vec4 out_color;

void main()
{
    vec2 center_offset = var_texcoord0 * 2.0 - 1.0;
    float r2 = dot(center_offset, center_offset);

    if (r2 > 1.0) discard;

    out_color = vec4(0.1, 0.0, 0.0, 0.0);
}

