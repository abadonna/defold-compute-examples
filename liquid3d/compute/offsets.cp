#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D spatial;

#include "/compute/common.glsl"

uint key_at(uint index) {
    ivec2 coords = index_to_coords(index);
    return uint(imageLoad(spatial, coords).x);
}

void main()
{
    uint i = gl_GlobalInvocationID.x;
    uint num_particles = uint(options.w);
    
    if (i >= num_particles) return;

    uint key = key_at(i);
    
    uint prev = (i == 0) ? num_particles : key_at(i - 1);
    
    if (key != prev) {
        ivec2 coords = index_to_coords(key);
        uvec4 data = uvec4(imageLoad(spatial, coords));
        data.y = i;
        imageStore(spatial, coords, data);
    }
    
}
