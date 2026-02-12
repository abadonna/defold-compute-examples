#version 140
in mediump vec2 var_texcoord0;
in mediump vec3 var_color;

out vec4 out_fragColor;

uniform fs_uniforms
{
    mediump vec4 tint;
};

void main()
{
    vec2 coord = var_texcoord0 * 2.0 - 1.0;

    float dist = length(coord);
    if (dist > 1.0) discard; // Cut out the corners of the square

    // Make it a nice soft glow
    float alpha = 1.0 - smoothstep(0.8, 1.0, dist);

    out_fragColor = vec4(var_color, alpha);
}

