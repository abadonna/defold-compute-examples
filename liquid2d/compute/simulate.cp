#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D points;
layout(rgba32f) uniform image2D predict;
layout(rgba32f) uniform image2D texture_out;

uniform uniforms
{
    vec4 options; // x - dt, y - gravity
};

int particle_radius = 7;
float damping = .8;

void draw_circle(vec2 pos, vec3 color) {
    int smoothing = 3;
    for (int x = -particle_radius; x <= particle_radius; x++) {
        for (int y = -particle_radius; y <= particle_radius; y++) {
            vec2 point = vec2(pos.x + x, pos.y + y);
            float dist = distance(pos, point);
           
            if (dist <= particle_radius) {
                float alpha = 1.0 - smoothstep(particle_radius - smoothing, particle_radius, dist);
                vec4 old_color = imageLoad(texture_out, ivec2(point));
                vec3 blended = mix(old_color.rgb, color, alpha);
                alpha = old_color.a + alpha * (1.0 - old_color.a); 
                
                imageStore(texture_out, ivec2(point), vec4(blended.rgb, alpha));
            }
        }
    }
}

void resolve_collisions(inout vec2 pos, inout vec2 vel) {
    vec2 half_bounds_size = vec2(512, 512);
    vec2 check = pos - half_bounds_size;

    if (abs(check.x) > half_bounds_size.x) {
        pos.x = half_bounds_size.x * sign(check.x) + half_bounds_size.x;
        vel.x *= -1 * damping;
    }
    
    if (abs(check.y) > half_bounds_size.y) {
        pos.y = half_bounds_size.y * sign(check.y) + half_bounds_size.y;
        vel.y *= -1 * damping;
    }

}

vec3 gradient(float t) {
    vec3 blue = vec3(.12, .32, .62);
    vec3 green = vec3(.38, .99, .62);
    vec3 yellow = vec3(.98, .94, .01);
    vec3 orange = vec3(.89, .36, .09);

    if (t < .5) {
        return mix(blue, green, t * 2);
    }

    if (t < .75) {
        return mix(green, yellow, (t - .5) * 4);
    }

    return mix(yellow, orange, (t - .75) * 4);
}

void main()
{
    float dt = options.x;
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 data = imageLoad(points, texelCoord);
    vec2 velocity = data.zw;
    vec2 position = data.xy;
    
    position += velocity * dt; 
    resolve_collisions(position, velocity);

    float v = smoothstep(0, 250, length(velocity));
   
    draw_circle(position, gradient(v));
    imageStore(points, texelCoord, vec4(position, velocity));
}
