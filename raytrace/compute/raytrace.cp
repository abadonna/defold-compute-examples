#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D tex_output;
layout(rgba32f) readonly uniform restrict image2D tex_positions;
layout(rgba32f) readonly uniform restrict image2D tex_normals;

uniform uniforms
{
    vec4 options; // width, height, face count, data texture size
};

vec3 light = vec3(0., 2., 2.);

vec3 get_position(int idx) {
    int size = int(options.w);
    int y = idx / size;
    int x = idx - y * size;
    return imageLoad(tex_positions, ivec2(x,y)).xyz;
}

vec3 get_normal(int idx) {
    int size = int(options.w);
    int y = idx / size;
    int x = idx - y * size;
    return imageLoad(tex_normals, ivec2(x,y)).xyz;
}

bool rayTriangleIntersect( //scratchapixel.com
    vec3 orig, vec3 dir,
    vec3 v0, vec3 v1, vec3 v2,
    out float t, out float u, out float v) {

    float kEpsilon = 1e-8;

    // compute the plane's normal
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    // no need to normalize
    vec3 N = cross(v0v1, v0v2); // N
    float denom = dot(N, N);

    // Step 1: finding P

    // check if the ray and plane are parallel.
    float NdotRayDirection = dot(N, dir);

    if (abs(NdotRayDirection) < kEpsilon) // almost 0
    return false; // they are parallel so they don't intersect! 

    // compute d parameter using equation 2
    float d = - dot(N, v0);

    // compute t (equation 3)
    t = -(dot(N, orig) + d) / NdotRayDirection;

    // check if the triangle is behind the ray
    if (t < 0) return false; // the triangle is behind

    // compute the intersection point using equation 1
    vec3 P = orig + vec3(t * dir.x, t * dir.y, t * dir.z);

    // Step 2: inside-outside test
    vec3 C; // vector perpendicular to triangle's plane

    // edge 0
    vec3 edge0 = v1 - v0; 
    vec3 vp0 = P - v0;
    C = cross(edge0, vp0);
    if (dot(N, C) < 0) return false; // P is on the right side


    // edge 1
    vec3 edge1 = v2 - v1; 
    vec3 vp1 = P - v1;
    C = cross(edge1, vp1);
    u = dot(N, C);
    if (u < 0)  return false; // P is on the right side


    // edge 2
    vec3 edge2 = v0 - v2; 
    vec3 vp2 = P - v2;
    C = cross(edge2, vp2);
    v = dot(N, C);
    if (v < 0) return false; // P is on the right side;

    u /= denom;
    v /= denom;

    return true; // this ray hits the triangle
}

int intersect(vec3 orig, vec3 dir, inout float dist, out vec2 uv) 
{
    int face = -1;
    int face_count = int(options.z);
    
    float u, v, t;
    for (int i = 0; i < face_count; i ++) {
        int k = i * 3;
        vec3 v0 = get_position(k);
        vec3 v1 = get_position(k + 1);
        vec3 v2 = get_position(k + 2);

        if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v) && t < dist) {
            dist = t;
            face = i;
            uv.x = u;
            uv.y = v;
        }
    }
    return face;
}

float cast_ray(vec3 orig, vec3 dir) 
{
    float result = 0.0;
    float dist = 3.402823466e+38; //FLT_MAX
    vec2 uv;

    int face = intersect(orig, dir, dist, uv);

    if (face > -1) {
   
        vec3 point = orig + dir * dist;
        vec3 normal = (1 - uv.x - uv.y) * get_normal(face * 3) + uv.x * get_normal(face * 3 + 1) + uv.y * get_normal(face * 3 + 2);
        normal = normalize(normal);
        vec3 light_dir = normalize(light - point);

        result =  max(0., dot(normal, light_dir));
    }
    
    return result;
}

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    float aspect = 1.5;
    float tan_hfov = tan(0.7854 * 0.5);
    int width = int(options.x);
    int height = int(options.y);
    
    vec3 p = vec3(texelCoord.x / float(width - 1) - 0.5, texelCoord.y / float(height - 1) - 0.5, 0); 

    p *= tan_hfov;
    p.x *= aspect;
    p.z = 9;

    vec3 origin = vec3(0, 0, 10); //camera position 
    vec3 dir = normalize(p - origin);

    float value = cast_ray(origin, dir);
    
    imageStore(tex_output, texelCoord, vec4(value, value, value, 1));
}
