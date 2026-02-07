#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D spatial;

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
    vec4 point = imageLoad(predict, coords);
    vec2 position = point.xy;
    vec2 velocity = point.zw;

    vec2 force = vec2(0);
    ivec2 cell = get_cell(position);

    //if (texelCoord != pointCoord) calculate(pointCoord);
    
    for (int i = 0; i < 9; i++)
    {
        uint hash = hash_cell(cell + offsets[i]);
        uint key = cell_key(hash);
        uint index = uint(imageLoad(spatial, index_to_coords(key)).y);

        while (index < num_particles)
        {
            uint neighbour_index = index;
            index ++;

            // Skip if looking at self
            if (neighbour_index == particle) continue;

            ivec2 neighbour_coords = index_to_coords(neighbour_index);

            uint neighbour_key = uint(imageLoad(spatial, neighbour_coords).x); 
            // Exit if no longer looking at correct bin
            if ((neighbour_key != key)) break;

            vec4 data = imageLoad(predict, neighbour_coords);
            vec2 neighbour_position = data.xy;
            vec2 neighbour_velocity = data.zw;

            vec2 offset = neighbour_position - position;
            float dst = length(offset);

            // Skip if not within radius
            if (dst > radius) continue;

            force += (neighbour_velocity - velocity) * smoothing_viscosity_kernel(dst);
        }

    }

    
    float dt = options.x;
    velocity += force * viscosity_strength * dt;
    position = imageLoad(points, coords).xy;
    imageStore(points, coords, vec4(position, velocity));

}
