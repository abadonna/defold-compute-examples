#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D tex_velocity;

#include "/compute/common.glsl"

float damping = .7;

void resolve_collisions(inout vec3 pos, inout vec3 vel) {
    float half_bounds_size = display.x / 2;

    if (abs(pos.x) > half_bounds_size) {
        pos.x = half_bounds_size * sign(pos.x);
        vel.x *= -1 * damping;
    }
    
    if (abs(pos.y) > half_bounds_size) {
        pos.y = half_bounds_size * sign(pos.y);
        vel.y *= -1 * damping;
    }

    if (abs(pos.x) > half_bounds_size) {
        pos.x = half_bounds_size * sign(pos.x);
        vel.x *= -1 * damping;
    }

    if (abs(pos.z) > half_bounds_size) {
        pos.z = half_bounds_size * sign(pos.z);
        vel.z *= -1 * damping;
    }

}

void main()
{
    float dt = options.x;
    ivec2 coords = index_to_coords(gl_GlobalInvocationID.x);

    vec3 position = imageLoad(points, coords).xyz;
    vec3 velocity = imageLoad(tex_velocity, coords).xyz;
    
    position += velocity * dt; 
    resolve_collisions(position, velocity);

    imageStore(points, coords, vec4(position, 0));
    imageStore(tex_velocity, coords, vec4(velocity, 0));
}
