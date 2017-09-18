#version 420

uniform(binding = 0) mat4 model;
uniform(binding = 1) mat4 view;
uniform(binding = 2) mat4 projection;

layout(location = 0) vec3 in_pos;

void main() {
	gl_Position = projection * (view * (model * vec4(in_pos, 1)));
}