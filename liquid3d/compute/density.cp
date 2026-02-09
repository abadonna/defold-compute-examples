#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D texture_density;
layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D spatial;


#include "/compute/common.glsl"

float scaling_factor = 0.0008;
float smoothing_kernel(float dst) {
    
    if (dst < radius)
    {
        float v = radius - dst;
        return v * v * scaling_factor;
    }
    return 0;
}

void main()
{
    uint particle = gl_GlobalInvocationID.x;
    uint num_particles = uint(options.w);

    if (particle >= num_particles) return;

    ivec2 coords = index_to_coords(particle);
    vec3 position = imageLoad(predict, coords).xyz;
    ivec3 cell = get_cell(position);
    
    float density = 0;
  
    for (int i = 0; i < 27; i++)
    {
        uint hash = hash_cell(cell + offsets[i]);
        uint key = cell_key(hash);
        uint index = uint(imageLoad(spatial, index_to_coords(key)).y);

        while (index < num_particles)
        {
            uint neighbour_index = index;
            index ++;

            ivec2 neighbour_coords = index_to_coords(neighbour_index);
            uint neighbour_key = uint(imageLoad(spatial, neighbour_coords).x); 
            
            // Exit if no longer looking at correct bin
            if ((neighbour_key != key)) break;

            vec3 neighbour_position = imageLoad(predict, neighbour_coords).xyz;
            float dst = length(neighbour_position - position);

            // Skip if not within radius
            if (dst > radius) continue;
            density += smoothing_kernel(dst);
        }
    }
 
    imageStore(texture_density, coords, vec4(density, 0, 0, 0));

}
