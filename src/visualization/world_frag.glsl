#version 330

varying vec2 coords; // fragment in question
uniform sampler2D tex; // texture
uniform float t; // time component
uniform int STEPS; // The number of adjacent locations in one direction to use for smearing

// function that does not perform any transform

//function to generate vector field
vec2 f4(float x, float y) {
	return vec2(0.5*sin(5.0*x*x*cos(t) + 5.0*y*sqrt(abs(y))) + y, 0.4*cos(5.0*x*x*sin(t) + 5.0*y*sqrt(abs(y))) - x);
}

// function to use for the x and y portions of the vector field
vec2 f(float x, float y) {
	return f4(x, y);
}

// perfrom line integral convolution
vec4 lic() {
	const float STEPSIZE = 0.01; // Stepsize to access adjacent texture sample

	vec2 init_coords = (2.0*coords)-vec2(1.0,1.0); // inital x,y coordinate, shift domain from (0,1) domain to (-1,1)
	vec2 v; // the vector field's x and y components
	vec2 step_coords; // current step x,y coordinate
	vec4 texture_value = (0.0,0.0,255.0,0.0); // color value at the particular texture coordinate vec4/RGBA
	float running_total = 0.0; // running total of grey-scale texture values

	// Start at this fragment's location
	step_coords = init_coords;

	// step FORWARD along the vector field
	for(int i=0; i<STEPS; i++) {
		v = f(step_coords[0], step_coords[1]);

		// use Euler's Method. Get the next approximate point along the curve
		step_coords += STEPSIZE * v;

		// get the texture value at that point
		texture_value = texture2D(tex, step_coords); // color value at the particular texture coordinate vec4/RGBA
		running_total += texture_value[2]; // It is a grayscale texture, so just use R in RGBA
	}

	// resart at this fragment's location
	step_coords = init_coords;

	// step BACKWARD along the vector field
	for(int i=0; i<STEPS; i++) {
		v = f(step_coords[0], step_coords[1]);

		// use Euler's Method. Get the next approximate point along the curve
		step_coords -= STEPSIZE * v;

		// get the texture value at that point
		texture_value = texture2D(tex, step_coords); // color value at the particular texture coordinate vec4/RGBA
		running_total += texture_value[0]; // It is a grayscale texture, so just use R in RGBA
	}

	// average grayscale value of all samples along the curve, this is the final value for this pixel
	float average_value = running_total / float(STEPS * 2);

	// use average value for RGB and 1 for A
	vec4 return_value = vec4(average_value,average_value,average_value,1.0);

	//if( init_coords[0] > 0.25 && init_coords[0] < 0.25 && init_coords[1] > 0.25 && init_coords[1] > 0.25 )
	if((init_coords[0] * init_coords[0] + init_coords[1] * init_coords[1] )< 0.25 )
		return (0.0,0.0,0.0,1.0);

	return return_value;
}

layout(location = 0) vec4 out_color;

void main() {
	gl_FragColor = lic();   // run lic; note the return value is of type vec4/RGBA
	out_color = vec4(1, 1, 1, 1);
}
