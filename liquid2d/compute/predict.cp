#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D predict;

uniform uniforms
{
    vec4 options; // x - dt, y - gravity
    vec4 mouse; // xy - pos, w - strength
};

float damping = .8;
float input_radius = 100.;

void main()
{
    float dt = options.x;
    float gravity = options.y;
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec4 data = imageLoad(points, texelCoord);
    vec2 position = data.xy; 
    vec2 velocity = data.zw;
    vec2 acceleration = vec2(0, -1) * gravity;
    
    if (mouse.w != 0) {
        vec2 offset = mouse.xy - position;
        float dst = length(offset);
        if (dst < input_radius)
        {
            vec2 dir = offset / dst;
            float center = 1 - dst / input_radius;
            
            acceleration *= dir * center * mouse.w;
            acceleration -= velocity * center;
        }
    }

    velocity += acceleration * dt;
    position += velocity * dt;
    
    imageStore(predict, texelCoord, vec4(position, velocity));
}
