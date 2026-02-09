#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D spatial;
layout(rgba32f) uniform image2D tex_velocity;

#include "/compute/common.glsl"

float scaling_factor = 1;
float viscosity_strength = .00000000002;
float smoothing_viscosity_kernel(float dst) {
    float v = radius * radius - dst * dst;
    return v * v * v * scaling_factor;
}

void main()
{
    uint particle = gl_GlobalInvocationID.x;
    uint num_particles = uint(options.w);

    if (particle >= num_particles) return;

    ivec2 coords = index_to_coords(particle);
    vec3 position = imageLoad(predict, coords).xyz;
    vec3 velocity = imageLoad(tex_velocity, coords).xyz;

    vec3 force = vec3(0);
    ivec3 cell = get_cell(position);
    
    for (int i = 0; i < 27; i++)
    {
        uint hash = hash_cell(cell + offsets[i]);
        uint key = cell_key(hash);
        uint index = uint(imageLoad(spatial, index_to_coords(key)).y);

        while (index < num_particles)
        {
            uint neighbour_index = index;
            index ++;

            // Skip if self
            if (neighbour_index == particle) continue;

            ivec2 neighbour_coords = index_to_coords(neighbour_index);

            uint neighbour_key = uint(imageLoad(spatial, neighbour_coords).x); 
            // Exit if no longer looking at correct bin
            if ((neighbour_key != key)) break;

            vec3 neighbour_position = imageLoad(predict, neighbour_coords).xyz;
            vec3 neighbour_velocity = imageLoad(tex_velocity, neighbour_coords).xyz;

            vec3 offset = neighbour_position - position;
            float dst = length(offset);

            // Skip if not within radius
            if (dst > radius) continue;

            force += (neighbour_velocity - velocity) * smoothing_viscosity_kernel(dst);
        }

    }

    float dt = options.x;
    velocity += force * viscosity_strength * dt;
   
    imageStore(tex_velocity, coords, vec4(velocity, 0));

}
