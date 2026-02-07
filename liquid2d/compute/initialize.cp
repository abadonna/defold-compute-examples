#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;

#include "/compute/common.glsl"

void main()
{
    // 1. Get Grid Coordinates
    // Using direct X/Y from your Lua 2D dispatch (32, 32, 1)
    uint col = gl_GlobalInvocationID.x;
    uint row = gl_GlobalInvocationID.y;

    uint grid_dim = uint(options.z);

    // 2. Calculate Spacing
    float screen_size = display.x;

    // Fit to screen, BUT cap at 20.0 pixels max
    float fit_spacing = screen_size / float(grid_dim);
    float spacing = min(20.0, fit_spacing);

    // 3. Calculate Centering Offset
    // If spacing is 20, the block might be smaller than the screen.
    // We calculate how much empty space is left and split it by 2.
    float total_block_size = float(grid_dim) * spacing;
    float offset = (screen_size - total_block_size) * 0.5;

    // 4. Calculate Position
    float half_spacing = spacing * 0.5;

    // Start at offset, then move by col/row
    float x = offset + (float(col) * spacing) + half_spacing;
    float y = offset + (float(row) * spacing) + half_spacing;

    // 5. Store
    imageStore(points, ivec2(col, row), vec4(x, y, 0, 0));
}
