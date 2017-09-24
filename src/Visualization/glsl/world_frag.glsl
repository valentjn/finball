#version 330
#extension GL_ARB_explicit_uniform_location : require

uniform sampler2D tex;
layout(location = 3) uniform int mode;

in vec3 frag_color;

layout(location = 0) out vec4 out_color;

void main() {
    if (mode == 0)
	    out_color = vec4(frag_color, 1);
    else
        out_color = texture(tex, frag_color.xy);
}
