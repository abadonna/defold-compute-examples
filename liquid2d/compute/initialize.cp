#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;

#include "/compute/common.glsl"

void main()
{
    uint col = gl_GlobalInvocationID.x;
    uint row = gl_GlobalInvocationID.y;

    uint grid_dim = uint(options.z);

    float screen_size = display.x;

    float fit_spacing = screen_size / float(grid_dim);
    float spacing = min(10.0, fit_spacing);

    float total_block_size = float(grid_dim) * spacing;
    float offset = (screen_size - total_block_size) * 0.5;

    float half_spacing = spacing * 0.5;

    float x = offset + (float(col) * spacing) + half_spacing;
    float y = offset + (float(row) * spacing) + half_spacing;

    imageStore(points, ivec2(col, row), vec4(x, y, 0, 0));
}
