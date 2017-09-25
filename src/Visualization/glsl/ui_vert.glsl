#version 330
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform mat4 model;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

out vec3 frag_color;

void main() {
	gl_Position = model * vec4(in_pos, 1);
    frag_color = in_color;
}
