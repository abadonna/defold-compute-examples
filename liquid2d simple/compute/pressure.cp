#version 430

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D texture_density;

uniform uniforms
{
    vec4 options; // x - dt, y - gravity
};

float radius = 50;
float scaling_factor = 1.;
float smoothing_kernel_derivative(float dst) {
    float v = radius - dst;
    return -v * scaling_factor;
}

float target_density = 5;
float pressure_multiplier = 800;

float pressure_from_density(float density) {
    float error = density - target_density;
    return error * pressure_multiplier;
}

float calc_shared_pressure(float d1, float d2) {
    float p1 = pressure_from_density(d1);
    float p2 = pressure_from_density(d2);
    return (p1 + p2) / 2;
}

shared vec2 sample_position;
shared vec2 sample_velocity;
shared float sample_density;
shared vec2[1024] pressure;

void calculate(ivec2 pointCoord) {
    vec4 point = imageLoad(predict, pointCoord);
    vec2 position = point.xy;
    vec2 offset = position - sample_position;
    float dst = length(offset);

    if (dst > radius) {
        pressure[gl_LocalInvocationIndex] = vec2(0);
    } else {
        vec2 dir = (dst == 0) ? vec2(0, 1) : offset / dst;
        float slope = smoothing_kernel_derivative(dst);
        float density = imageLoad(texture_density, pointCoord).x;
        float shared_pressure = calc_shared_pressure(density, sample_density);
        pressure[gl_LocalInvocationIndex] = shared_pressure * dir * slope  / density;
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
        sample_density = imageLoad(texture_density, texelCoord).x;
    }
    
    barrier();

    if (texelCoord != pointCoord) calculate(pointCoord);
    
    barrier();

    // Parallel Reduction
    uint idx = gl_LocalInvocationIndex;
    for (uint s = 512; s > 0; s >>= 1) {
        if (idx < s) {
            pressure[idx] += pressure[idx + s];
        }
        barrier();
    }
 
    if (gl_LocalInvocationIndex == 0) {
        float dt = options.x;
        vec2 acceleration = pressure[0] / sample_density;
        sample_velocity += acceleration * dt;
        imageStore(predict, texelCoord, vec4(sample_position, sample_velocity));
    }

}
