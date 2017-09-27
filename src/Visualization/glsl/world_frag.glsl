#version 330
#extension GL_ARB_explicit_uniform_location : require

uniform sampler2D tex;
layout(location = 3) uniform int mode;

in vec3 frag_color;
in float frag_depth;

layout(location = 0) out vec4 out_color;

void main()
{
    if (mode == 0)
	    out_color = vec4(frag_color, 1);
    else {
        out_color = vec4(texture(tex, frag_color.xy).xyz, 1);
		if (out_color.a == 0)
			discard;
	}

    gl_FragDepth = frag_depth;
}
