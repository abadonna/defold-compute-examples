#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D tex_velocity;

#include "/compute/common.glsl"

float damping = .7;

vec3 rotate_vector(vec3 v, vec4 q) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void resolve_collisions(inout vec3 pos, inout vec3 vel) {
    float half_bounds_size = display.x / 2;

    vec4 inv = vec4(-collider.xyz, collider.w);
    
    vec3 local_pos = rotate_vector(pos, inv);
    vec3 local_vel = rotate_vector(vel, inv);

    bool collided = false;

    if (abs(local_pos.x) > half_bounds_size) {
        local_pos.x = half_bounds_size * sign(local_pos.x);
        local_vel.x *= -1 * damping;
        collided = true;
    }
    
    if (abs(local_pos.y) > half_bounds_size) {
        local_pos.y = half_bounds_size * sign(local_pos.y);
        local_vel.y *= -1 * damping;
        collided = true;
    }

    if (abs(local_pos.z) > half_bounds_size) {
        local_pos.z = half_bounds_size * sign(local_pos.z);
        local_vel.z *= -1 * damping;
        collided = true;
    }

    if (collided) {
        pos = rotate_vector(local_pos, collider);
        vel = rotate_vector(local_vel, collider);
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
