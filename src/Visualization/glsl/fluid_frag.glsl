#version 330
#extension GL_ARB_explicit_uniform_location : require

uniform sampler2D tex_vecs; // velocities texture
uniform sampler2D tex_noise; // noise texture

const int STEPS=10; // The number of adjacent locations in one direction to use for smearing

// perfrom line integral convolution
float lic(vec2 normalized_coords) {  
	const float STEPSIZE = 0.01; // Stepsize to access adjacent texture sample

	//vec2 init_coords = (2.0*coords)-vec2(1.0,1.0); // inital x,y coordinate, shift domain from (0,1) domain to (-1,1)
	vec2 v; // the vector field's x and y components
	vec2 step_coords; // current step x,y coordinate
	vec4 texture_value = vec4(0.0,0.0,0.0,0.0); // color value at the particular texture coordinate vec4/RGBA
	float running_total = 0.0; // running total of grey-scale texture values

	// Start at this fragment's location
	step_coords = normalized_coords;
	
	// step FORWARD along the vector field
	for(int i=0; i<STEPS; i++) {
		v = texture(tex_vecs, normalized_coords).xy;

		// use Euler's Method. Get the next approximate point along the curve
		step_coords += STEPSIZE * v;

		// get the texture value at that point
		texture_value = texture2D(tex_noise, step_coords); // color value at the particular texture coordinate vec4/RGBA
		running_total += texture_value[2]; // It is a grayscale texture, so just use R in RGBA
	}

	// resart at this fragment's location
	step_coords = normalized_coords;

	// step BACKWARD along the vector field
	for(int i=0; i<STEPS; i++) {
		v = texture(tex_vecs, normalized_coords).xy;

		// use Euler's Method. Get the next approximate point along the curve
		step_coords -= STEPSIZE * v;

		// get the texture value at that point
		texture_value = texture2D(tex_noise, step_coords); // color value at the particular texture coordinate vec4/RGBA
		running_total += texture_value[0]; // It is a grayscale texture, so just use R in RGBA
	}

	// average grayscale value of all samples along the curve, this is the final value for this pixel
	float average_value = running_total / float(STEPS * 2);

	return average_value;
}


layout(location = 0) out vec4 out_color;

void main() {
    vec2 normalized_coords = gl_FragCoord.xy / textureSize(tex_noise, 0);
    ivec2 vecs_res = textureSize(tex_vecs, 0);
    normalized_coords = (normalized_coords * vecs_res + 0.5) / (vecs_res + 1);
    out_color = vec4(lic(normalized_coords));   // run lic; note the return value is of type vec4/RGBA
}

