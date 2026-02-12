#version 140

out vec4 frag_color;
in vec2 var_tex;
uniform sampler2D tex0;

uniform fs_uniforms
{
	uniform highp mat4 mtx_proj;
	highp vec4 params; //xy -offset, z - width, w - mask
};

// --- TUNING PARAMETERS ---
// Radius: Larger = Smoother, but slower.
// For nice fluid, you typically need 10-20 pixels.
const float WORLD_RADIUS = 4; //world radius
const int MAX_SCREEN_RADIUS = 30; //max screen radius

const float STRENGTH = .4;

// Depth Falloff: How much we hate depth differences.
// Higher number = Sharper edges (stops blurring sooner).
// Lower number = Blurry edges.
// TUNE THIS if your fluid looks like marbles (decrease it) or fog (increase it).
const float SHARPNESS = 0.002; 

//https://github.com/SebLague/Fluid-Sim/blob/main/Assets/Scripts/Rendering/ScreenSpace/Smoothing/Bilateral_1D/BilateralPass.hlsl
// Calculate the number of pixels covered by a world-space radius at given dst from camera
float CalculateScreenSpaceRadius(float depth, float imageWidth)
{
	float widthScale = mtx_proj[0][0]; // smaller values correspond to higher fov (objects appear smaller)
	float pxPerMeter = (imageWidth * widthScale) / (2 * depth);
	return abs(pxPerMeter) * WORLD_RADIUS;
}

void main()
{
	vec4 original = texture(tex0, var_tex);
	float depth = original.a;

	// Optimization: Don't blur background
	if (depth < 0.0001) {
		frag_color = vec4(0.0);
		return;
	}

	// Calculate screenspace radius
	float radiusFloat = CalculateScreenSpaceRadius(depth, params.z);
	int radius = int(ceil(radiusFloat));

	if (radius <= 1 && WORLD_RADIUS > 0) radius = 2;
	radius = min(MAX_SCREEN_RADIUS, radius);

	float fR = max(0, radius - radiusFloat); // use fractional part of radius in sigma calc to avoid harsh boundaries where radius integer changes
	float sigma = max(0.0000001, (radius - fR) / (6 * max(0.001, STRENGTH)));

	float weight_sum = 0.0;
	vec2 sum = vec2(0.0);
	
	for (int i = -radius; i <= radius; i++)
	{
		vec2 offset = params.xy * float(i);
		vec2 uv = var_tex + offset;

		vec4 sample_data = texture(tex0, uv);
		float sample_depth = sample_data.a;

		float spatial_dist = float(i);
		float w_spatial = exp(-(spatial_dist * spatial_dist) / (2.0 * sigma * sigma));

		float diff = depth - sample_depth;
		float w_range = exp(-diff * diff * SHARPNESS);

		float weight = w_spatial * w_range;

		sum += sample_data.xy * weight;
		weight_sum += weight;
	}

	if (weight_sum > 0.0) {
		sum /= weight_sum;
	}

	frag_color = vec4(mix(sum, original.xy, params.w), 0.0, depth);
}
