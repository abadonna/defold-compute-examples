#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D predict;

uniform uniforms
{
    vec4 options; // x - dt, y - gravity
};

float damping = .8;

void main()
{
    float dt = options.x;
    float gravity = options.y;
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 data = imageLoad(points, texelCoord);
    
    vec2 velocity = data.zw + vec2(0, -1) * gravity * dt;
    vec2 position = data.xy + velocity * dt; 
    
    imageStore(predict, texelCoord, vec4(position, velocity));
}
