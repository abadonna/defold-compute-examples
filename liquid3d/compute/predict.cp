#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D tex_velocity;
layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D spatial;

#include "/compute/common.glsl"

void main()
{
    uint i = gl_GlobalInvocationID.x;
    uint num_particles = uint(options.w);

    if (i >= num_particles) return;
    
    float dt = options.x;
    float gravity = options.y;
    
    ivec2 coord = index_to_coords(i);
    vec3 position = imageLoad(points, coord).xyz;
    vec3 velocity = imageLoad(tex_velocity, coord).xyz;
    vec3 acceleration = vec3(0, -1, 0) * gravity;

    velocity += acceleration * dt;
    position += velocity * dt;

    imageStore(predict, coord, vec4(position, 0));
    imageStore(tex_velocity, coord, vec4(velocity, 0));

    ivec3 cell = get_cell(position);
    uint hash = hash_cell(cell);
    uint key = cell_key(hash);

    imageStore(spatial, coord, uvec4(key, num_particles, 0, 0));
}
