uniform uniforms
{
	vec4 options; // k, j, texture size, num particles
	vec4 display;
};

ivec2 index_to_coords(uint idx) {
	uint size = uint(options.z);
	return ivec2(idx % size, idx / size);
}

float radius = 42;

const uint hashK1 = 15823;
const uint hashK2 = 9737333;
const uint hashK3 = 440817757;

const float PI = 3.1415926;

ivec3 get_cell(vec3 pos) {
	return ivec3(floor(pos / radius));
}

uint hash_cell(ivec3 cell)
{
	const uint block_size = 50;
	uvec3 ucell = uvec3(cell + block_size / 2);

	uvec3 local = ucell % block_size;
	uvec3 block_id = ucell / block_size;
	uint block_hash = block_id.x * hashK1 + block_id.y * hashK2 + block_id.z * hashK3;
	return local.x + block_size * (local.y + block_size * local.z) + block_hash;
}

uint cell_key(uint hash)
{
	return hash % uint(options.w);
}

const ivec3 offsets[27] =
{
	ivec3(-1, -1, -1),
	ivec3(0, -1, -1),
	ivec3(1, -1, -1),

	ivec3(-1, 0, -1),
	ivec3(0, 0, -1),
	ivec3(1, 0, -1),

	ivec3(-1, 1, -1),
	ivec3(0, 1, -1),
	ivec3(1, 1, -1),

	ivec3(-1, -1, 0),
	ivec3(0, -1, 0),
	ivec3(1, -1, 0),

	ivec3(-1, 0, 0),
	ivec3(0, 0, 0),
	ivec3(1, 0, 0),

	ivec3(-1, 1, 0),
	ivec3(0, 1, 0),
	ivec3(1, 1, 0),

	ivec3(-1, -1, 1),
	ivec3(0, -1, 1),
	ivec3(1, -1, 1),

	ivec3(-1, 0, 1),
	ivec3(0, 0, 1),
	ivec3(1, 0, 1),

	ivec3(-1, 1, 1),
	ivec3(0, 1, 1),
	ivec3(1, 1, 1)
};