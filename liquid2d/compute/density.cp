#version 430

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D texture_density;

float radius = 50;
float scaling_factor = 0.0004;
float smoothing_kernel(float dst) {
    
    if (dst < radius)
    {
        float v = radius - dst;
        return v * v * scaling_factor;
    }
    return 0;
}

shared float[1024] density;
shared vec2 origin;

void main()
{
    ivec2 texelCoord = ivec2(gl_WorkGroupID.xy);
    
    if (gl_LocalInvocationIndex == 0) {
        origin = imageLoad(predict, texelCoord).xy;
    }
    
    barrier();

    ivec2 pointCoord = ivec2(gl_LocalInvocationID.xy);
    vec2 point = imageLoad(predict, pointCoord).xy; 
    
    float dst = length(point - origin);
    density[gl_LocalInvocationIndex] = smoothing_kernel(dst);

    barrier();

    // Parallel Reduction
    uint idx = gl_LocalInvocationIndex;
    for (uint s = 512; s > 0; s >>= 1) {
        if (idx < s) {
            density[idx] += density[idx + s];
        }
        barrier();
    }
 
    if (gl_LocalInvocationIndex == 0) {
        float d = density[0];
        imageStore(texture_density, texelCoord, vec4(d, d, d, 1));
    }

}
