#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f) uniform image2D texture_out;

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    imageStore(texture_out, texelCoord, vec4(0));
}
