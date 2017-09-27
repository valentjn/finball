#version 330
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

out vec3 frag_color;
out float frag_depth;

void main() {
    vec4 pos = vec4(in_pos, 1);
    pos = model * pos;
    frag_depth = pos.z * 0.5 + 0.5;
    pos.z = 0;
	gl_Position = projection * (view * pos);
    frag_color = in_color;
}
