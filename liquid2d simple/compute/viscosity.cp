#version 430

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D points;

uniform uniforms
{
    vec4 options; // x - dt, y - gravity
};

float radius = 50;
float scaling_factor = 1;
float viscosity_strength = .00000000002;
float smoothing_viscosity_kernel(float dst) {
    float v = radius * radius - dst * dst;
    return v * v * v * scaling_factor;
}

shared vec2 sample_position;
shared vec2 sample_velocity;
shared vec2[1024] viscosity;

void calculate(ivec2 pointCoord) {
    vec4 point = imageLoad(predict, pointCoord);
    vec2 position = point.xy;
    vec2 velocity = point.zw;
    vec2 offset = position - sample_position;
    float dst = length(offset);

    if (dst > radius) {
        viscosity[gl_LocalInvocationIndex] = vec2(0);
    } else {
        vec2 dir = (dst == 0) ? vec2(0, 1) : offset / dst;
        viscosity[gl_LocalInvocationIndex] = (velocity - sample_velocity) * smoothing_viscosity_kernel(dst);
    }
}

void main()
{
    ivec2 texelCoord = ivec2(gl_WorkGroupID.xy);
    ivec2 pointCoord = ivec2(gl_LocalInvocationID.xy);
    
    if (gl_LocalInvocationIndex == 0) {
        vec4 point = imageLoad(predict, texelCoord);
        sample_position = point.xy;
        sample_velocity = point.zw;
    }

    barrier();

    if (texelCoord != pointCoord) calculate(pointCoord);
    
    barrier();

    // Parallel Reduction
    uint idx = gl_LocalInvocationIndex;
    for (uint s = 512; s > 0; s >>= 1) {
        if (idx < s) {
            viscosity[idx] += viscosity[idx + s];
        }
        barrier();
    }
 
    if (gl_LocalInvocationIndex == 0) {
        float dt = options.x;
        vec2 velocity = sample_velocity + viscosity[0] * viscosity_strength * dt;
        vec2 position = imageLoad(points, texelCoord).xy;
        imageStore(points, texelCoord, vec4(position, velocity));
    }

}
