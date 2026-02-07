#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D spatial;
layout(rgba32f) uniform image2D texture_density;

#include "/compute/common.glsl"

float scaling_factor = 1.;
float smoothing_kernel_derivative(float dst) {
    float v = radius - dst;
    return -v * scaling_factor;
}

float target_density = 5;
float pressure_multiplier = 800;

float pressure_from_density(float density) {
    float error = density - target_density;
    return error * pressure_multiplier;
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
    float density = imageLoad(texture_density, coords).x;

    float pressure = pressure_from_density(density);
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

            vec2 neighbour_position = imageLoad(predict, neighbour_coords).xy;

            vec2 offset = neighbour_position - position;
            float dst = length(offset);

            // Skip if not within radius
            if (dst > radius) continue;

            vec2 dir = (dst == 0) ? vec2(0, 1) : offset / dst;
            float slope = smoothing_kernel_derivative(dst);
            float neighbour_density = imageLoad(texture_density, neighbour_coords).x;
            float neighbour_pressure = pressure_from_density(neighbour_density);
            
            float shared_pressure = (pressure + neighbour_pressure) / 2;
            force += shared_pressure * dir * slope  / neighbour_density;
            
        }
    }
 
    float dt = options.x;
    vec2 acceleration = force / density;
    velocity += acceleration * dt;
    imageStore(predict, coords, vec4(position, velocity));
   
}
