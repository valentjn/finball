#version 330
#extension GL_ARB_explicit_uniform_location : require

layout(location = 3) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

layout(location = 0) in vec3 in_pos;

const vec3 verts[6] = vec3[](
	vec3(-0.5, -0.5, 0),
	vec3(-0.5, 0.5, 0),
	vec3(0.5, -0.5, 0),
	vec3(-0.5, -0.5, 0),
	vec3(-0.5, 0.5, 0),
	vec3(0.5, -0.5, 0)
);

void main() {
	gl_Position = projection * (view * (model * vec4(in_pos, 1)));
}
