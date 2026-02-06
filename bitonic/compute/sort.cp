#version 430

layout (local_size_x = 512, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;

uniform uniforms
{
    vec4 options; // k, j, texture size, num particles
};

ivec2 getTexel(uint idx) {
    uint size = uint(options.z);
    uint row = idx / size;
    return ivec2(idx - row * size, row);
}

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

        ivec2 at = getTexel(i);
        ivec2 bt = getTexel(ixj);
        
        uint a = uint(imageLoad(points, at).x);
        uint b = uint(imageLoad(points, bt).x);

        bool swap = ascending ? (a > b) : (a < b);
        if (swap) {
            imageStore(points, at, ivec4(b));
            imageStore(points, bt, ivec4(a));
        }
    }
    
}
