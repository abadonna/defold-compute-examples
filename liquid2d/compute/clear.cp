#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f) uniform image2D texture_out;

uniform uniforms
{
    vec4 display;
};


void main()
{
    uint size = uint(display.x);
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.x % size, gl_GlobalInvocationID.x / size);
    imageStore(texture_out, texelCoord, vec4(0));
}
