#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;

uniform uniforms
{
    vec4 rnd;
};

float random(vec2 seed) {
    return fract(sin(dot(seed.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 seed = vec2(gl_GlobalInvocationID.xy) + rnd.xy;

    //xy - position, zw - velocity
    //vec4 value = vec4(random(seed) * 1024, random(seed * 2) * 1024, 0, 0);
    float size = 20;
    float center = (1024 - 32 * size) /2.;
    vec4 value = vec4(gl_GlobalInvocationID.x * size + center, gl_GlobalInvocationID.y * size + center, 0, 0);

    imageStore(points, texelCoord, value);
}
