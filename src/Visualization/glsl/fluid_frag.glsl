#version 330
#extension GL_ARB_explicit_uniform_location : require

uniform sampler2D tex_vecs; // velocities texture
uniform sampler2D tex_noise; // noise texture
uniform sampler2D tex_density; // density texture

const int STEPS=10; // The number of adjacent locations in one direction to use for smearing

// perfrom line integral convolution
float lic(vec2 normalized_coords) {
	vec2 texel_size = 1.0 / textureSize(tex_noise, 0);

	vec2 v; // the vector field's x and y components
	vec2 step_coords; // current step x,y coordinate
	vec4 texture_value = vec4(0.0,0.0,0.0,0.0); // color value at the particular texture coordinate vec4/RGBA
	float running_total = 0.0; // running total of grey-scale texture values

	// Start at this fragment's location
	step_coords = normalized_coords;
	
	// step FORWARD along the vector field
	for(int i=0; i < STEPS; i++) {
		v = texture(tex_vecs, normalized_coords).xy;

		// use Euler's Method. Get the next approximate point along the curve
		step_coords += v * texel_size;

		// get the texture value at that point
		running_total += texture(tex_noise, step_coords).x;
	}

	// resart at this fragment's location
	step_coords = normalized_coords;

	// step BACKWARD along the vector field
	for(int i=0; i<STEPS; i++) {
		v = texture(tex_vecs, normalized_coords).xy;

		// use Euler's Method. Get the next approximate point along the curve
		step_coords -= v * texel_size;

		// get the texture value at that point
		running_total += texture(tex_noise, step_coords).x;
	}

	// average grayscale value of all samples along the curve, this is the final value for this pixel
	float average_value = running_total / float(STEPS * 2);

	return average_value;
}


layout(location = 0) out vec4 out_color;

void main() {
    vec2 normalized_coords = gl_FragCoord.xy / (textureSize(tex_noise, 0) * 2);
    ivec2 vecs_res = textureSize(tex_vecs, 0);
    normalized_coords = (normalized_coords * vecs_res + 0.5) / (vecs_res + 1);
    //out_color = vec4(0.2,0.8,1,1) * lic(normalized_coords);   // run lic; note the return value is of type vec4/RGBA
    float velocity_val = lic(normalized_coords);
    float density_val = texture(tex_vecs, normalized_coords).z;
    if (density_val < 1)
        out_color = vec4(0, 0, 1, 1) * (1 - density_val) + vec4(0, 1, 0, 1) * density_val;
    else {
        density_val -= 1;
        density_val = min(density_val, 1);
        out_color = vec4(0, 1, 0, 1) * (1 - density_val) + vec4(1, 0, 0, 1) * density_val;
    }
    out_color *= velocity_val;
}

