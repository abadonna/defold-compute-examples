uniform uniforms
{
	vec4 options; // k, j, texture size, num particles
	vec4 mouse; // xy - pos, w - strength
	vec4 display;
};

ivec2 index_to_coords(uint idx) {
	uint size = uint(options.z);
	return ivec2(idx % size, idx / size);
}

float radius = 50;
const uint hashK1 = 15823;
const uint hashK2 = 9737333;

ivec2 get_cell(vec2 pos) {
	return ivec2(floor(pos / radius));
}

uint hash_cell(ivec2 cell)
{
	uvec2 c = uvec2(cell);
	uint a = c.x * hashK1;
	uint b = c.y * hashK2;
	return (a + b);
}

uint hash_cell(vec2 cell)
{
	return hash_cell(ivec2(cell));
}

uint cell_key(uint hash)
{
	return hash % uint(options.w);
}

const ivec2 offsets[9] =
{
	ivec2(-1, 1),
	ivec2(0, 1),
	ivec2(1, 1),
	ivec2(-1, 0),
	ivec2(0, 0),
	ivec2(1, 0),
	ivec2(-1, -1),
	ivec2(0, -1),
	ivec2(1, -1),
};