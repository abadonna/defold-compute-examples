#version 140
in mediump vec2 var_texcoord0;
in mediump vec3 var_view_center; 
in mediump float var_radius;

out vec4 out_color;

uniform fs_uniforms
{
    uniform mediump mat4 mtx_proj2;
};


void main()
{
    vec2 center_offset = var_texcoord0 * 2.0 - 1.0;
    float r2 = dot(center_offset, center_offset);

    // Circle Shape: Discard pixels outside the radius
    if (r2 > 1.0) discard;

    // Calculate the Sphere (Z component of normal)
    // Sphere equation: x^2 + y^2 + z^2 = 1  =>  z = sqrt(1 - (x^2 + y^2))
    float n = sqrt(1.0 - r2);

    // Calculate the actual View Space Position of the sphere surface
    // View Space looks down negative Z. 
    // "Closer" to camera means Positive Z direction relative to the particle center.

    vec4 pixel_view_pos;
    pixel_view_pos.xy = var_view_center.xy + (center_offset * var_radius);
    pixel_view_pos.z  = var_view_center.z  + (n  * var_radius);
    pixel_view_pos.w  = 1.0;

    vec4 clip_pos = mtx_proj2 * pixel_view_pos;
    float ndc_depth = clip_pos.z / clip_pos.w;
    gl_FragDepth = ndc_depth * 0.5 + 0.5;

    // Calculate Linear Depth (Distance from Camera)
    // View Space Z is negative. Flip it to get positive distance.
    float linear_depth = -pixel_view_pos.z;
    out_color = vec4(linear_depth, 0.0, 0.0, 0.0);
}

