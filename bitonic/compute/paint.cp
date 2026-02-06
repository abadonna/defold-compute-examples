#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D texture_out;

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 data = imageLoad(points, texelCoord);
    float v = data.x / 255.;
    imageStore(texture_out, texelCoord, vec4(v, v, v, 1));
}
