#version 140

in highp vec4 var_position;

out vec4 out_fragColor;

uniform fs_uniforms
{
    mediump vec4 tint;
};

void main()
{
    out_fragColor = tint;
}

