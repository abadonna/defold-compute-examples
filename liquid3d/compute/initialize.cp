#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;

#include "/compute/common.glsl"

void main()
{
    // 1. Get Texture Coordinates
    uint col = gl_GlobalInvocationID.x;
    uint row = gl_GlobalInvocationID.y;
    uint tex_dim = uint(options.z);

    // 2. Flatten 2D ID to 1D Linear ID
    uint id = row * tex_dim + col;
    uint total_particles = uint(options.w);

    // 3. Calculate Grid Dimensions (Cube Root)
    float grid_side = ceil(pow(float(total_particles), 1.0/3.0));
    if (grid_side < 1.0) grid_side = 1.0; 

    // 4. Calculate Spacing
    float box_size = display.x;
    float spacing = min(box_size / grid_side, 20);

    // 5. Calculate Centering Offset
    // Calculate the actual physical size of the particle cloud
    float cloud_size = grid_side * spacing;

    // We want to center this cloud at 0,0,0.
    // Start at -HalfSize. Add HalfSpacing to center the point in its voxel.
    float start_offset = (cloud_size * -0.5) + (spacing * 0.5);

    // 6. Map Linear ID to 3D Grid (x, y, z)
    uint side = uint(grid_side);
    uint iz = id / (side * side);
    uint rem = id % (side * side);
    uint iy = rem / side;
    uint ix = rem % side;

    // 7. Calculate Final Position
    float px = start_offset + (float(ix) * spacing);
    float py = start_offset + (float(iy) * spacing);
    float pz = start_offset + (float(iz) * spacing);

    // 8. Write
    imageStore(points, ivec2(col, row), vec4(px, py, pz, 0.0));
}
