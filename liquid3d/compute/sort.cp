#version 430

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D spatial;
layout(rgba32f) uniform image2D tex_velocity;

#include "/compute/common.glsl"

void main()
{
    uint i = gl_GlobalInvocationID.x;
    uint num_particles = uint(options.w);
    
    if (i >= num_particles) return;
    uint j = uint(options.y);

    uint ixj = i ^ j; // partner index in this stage

    if (ixj > i && ixj < num_particles) {
        uint k = uint(options.x);
        bool ascending = ((i & k) == 0);

        ivec2 at = index_to_coords(i);
        ivec2 bt = index_to_coords(ixj);
        
        uint a = uint(imageLoad(spatial, at).x);
        uint b = uint(imageLoad(spatial, bt).x);

        bool need_swap = ascending ? (a > b) : (a < b);
        if (need_swap) {
            imageStore(spatial, at, vec4(b));
            imageStore(spatial, bt, vec4(a));
            
            vec4 pa = imageLoad(predict, at);
            vec4 pb = imageLoad(predict, bt);

            imageStore(predict, at, pb);
            imageStore(predict, bt, pa);

            pa = imageLoad(points, at);
            pb = imageLoad(points, bt);

            imageStore(points, at, pb);
            imageStore(points, bt, pa);

            pa = imageLoad(tex_velocity, at);
            pb = imageLoad(tex_velocity, bt);

            imageStore(tex_velocity, at, pb);
            imageStore(tex_velocity, bt, pa);
        }
    }
    
}
