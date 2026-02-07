#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D spatial;

#include "/compute/common.glsl"

float damping = .8;
float input_radius = 100.;

void main()
{
    uint i = gl_GlobalInvocationID.x;
    uint num_particles = uint(options.w);

    if (i >= num_particles) return;
    
    float dt = options.x;
    float gravity = options.y;
    
    ivec2 coord = index_to_coords(i);
    vec4 data = imageLoad(points, coord);
    vec2 position = data.xy; 
    vec2 velocity = data.zw;
    vec2 acceleration = vec2(0, -1) * gravity;

    /*
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
    }*/

    velocity += acceleration * dt;
    position += velocity * dt;
    
    imageStore(predict, coord, vec4(position, velocity));

    vec2 cell = get_cell(position);
    uint hash = hash_cell(cell);
    uint key = cell_key(hash);
    imageStore(spatial, coord, uvec4(key, num_particles, 0, 0));
}
