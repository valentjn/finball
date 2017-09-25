#version 330
#extension GL_ARB_explicit_uniform_location : require

uniform sampler2D tex_vecs; // velocities texture
uniform sampler2D tex_noise; // noise texture
uniform sampler2D tex_waves; // density texture
uniform float t;

const int[] perm = int[256](
	151, 160, 137, 91, 90, 15,
	131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
	190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
	102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
	5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
	223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
	129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
	251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
);

int hash(int i)
{
	return perm[i];
}

float grad(int hash, float x)
{
	int h = hash & 0x0F;			// Convert low 4 bits of hash code
	float grad = 1.0 + (h & 7);		// Gradient value 1.0, 2.0, ..., 8.0
	if ((h & 8) != 0) grad = -grad;	// Set a random sign for the gradient
									//float grad = gradients1D[h];	// NOTE : Test of Gradient look-up table instead of the above
	return (grad * x);				// Multiply the gradient with the distance
}

float grad(int hash, float x, float y)
{
	int h = hash & 0x3F;		// Convert low 3 bits of hash code
	vec2 uv = vec2(y, x);
	if (h < 4) uv = uv.yx;
	if ((h & 1) != 0) uv.x = -uv.x;
	if ((h & 2) != 0) uv.y = -uv.y;
	return dot(uv, vec2(1, 2));
}

float noise2D(vec2 coords)
{
	float n0, n1, n2;   // Noise contributions from the three corners

						// Skewing/Unskewing factors for 2D
	const float F2 = 0.366025403;  // F2 = (sqrt(3) - 1) / 2
	const float G2 = 0.211324865;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

									// Skew the input space to determine which simplex cell we're in
	float s = (coords.x + coords.y) * F2;  // Hairy factor for 2D
	vec2 coords_s = coords + s;
	vec2 ij_f = floor(coords_s);
	ivec2 ij = ivec2(ij_f);

	// Unskew the cell origin back to (x,y) space
	float t = dot(ij_f, vec2(1, 1)) * G2;
	vec2 X0Y0 = ij_f - t;
	vec2 x0y0 = coords - X0Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0y0.x > x0y0.y) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1;
		j1 = 0;
	}
	else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1 = 0;
		j1 = 1;
	}

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	vec2 x1y1 = x0y0 - ivec2(i1, j1) + G2;
	vec2 x2y2 = x0y0 - 1.0 + 2.0 * G2;

	// Calculate the contribution from the first corner
	float t0 = 0.5f - dot(x0y0, x0y0);
	if (t0 < 0.0) {
		n0 = 0.0;
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(hash(ij.x + hash(ij.y)), x0y0.x, x0y0.y);
	}

	// Calculate the contribution from the second corner
	float t1 = 0.5 - dot(x1y1, x1y1);
	if (t1 < 0.0) {
		n1 = 0.0;
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(hash(ij.x + i1 + hash(ij.y + j1)), x1y1.x, x1y1.y);
	}

	// Calculate the contribution from the third corner
	float t2 = 0.5 - dot(x2y2, x2y2);
	if (t2 < 0.0) {
		n2 = 0.0;
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(hash(ij.x + 1 + hash(ij.y + 1)), x2y2.x, x2y2.y);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 45.23065 * (n0 + n1 + n2);
}

vec2 getVeloc(vec2 coords) {
	return (texture(tex_vecs, coords).xy - 0.5) * 10;
}

const int STEPS=20; // The number of adjacent locations in one direction to use for smearing
const int velocity_scale=20;

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
		v = getVeloc(step_coords) * velocity_scale;

		// use Euler's Method. Get the next approximate point along the curve
		step_coords += v * texel_size;

		// get the texture value at that point
		running_total += texture(tex_noise, step_coords).x;
	}

	// resart at this fragment's location
	step_coords = normalized_coords;

	// step BACKWARD along the vector field
	for(int i=0; i<STEPS; i++) {
		v = getVeloc(step_coords) * velocity_scale;

		// use Euler's Method. Get the next approximate point along the curve
		step_coords -= v * texel_size;

		// get the texture value at that point
		running_total += texture(tex_noise, step_coords).x;
	}

	// average grayscale value of all samples along the curve, this is the final value for this pixel
	float average_value = running_total / float(STEPS * 2);

	return average_value;
}

layout(location = 0) out vec3 out_color;
layout(location = 1) out float out_wave;

void main() {
    ivec2 vecs_res = textureSize(tex_vecs, 0);
	ivec2 rend_res = textureSize(tex_noise, 0) * 2;

    vec2 normalized_coords = gl_FragCoord.xy / rend_res;
    normalized_coords = (normalized_coords * vecs_res + 0.5) / (vecs_res + 1);
    float velocity_val = lic(normalized_coords);
    float density_val = 2 * texture(tex_vecs, normalized_coords).z;
    if (density_val < 1)
        out_color = 2 * vec3(0, 0, 1) * (1 - density_val) + vec3(0, 1, 0) * density_val;
    else {
        density_val -= 1;
        density_val = min(density_val, 1);
        out_color = vec3(0, 1, 0) * (1 - density_val) + 2 * vec3(1, 0, 0) * density_val;
    }
    out_color *= velocity_val;

	vec2 veloc = getVeloc(normalized_coords);

	float noise_scale = 2.5;
	float tscale = 4;

	float prev_out_wave = texture(tex_waves, normalized_coords).x;

	if (int(gl_FragCoord.x) <= 1)
		out_wave = noise2D(vec2(noise_scale * normalized_coords.y, t * tscale));
	else if (int(gl_FragCoord.x) >= rend_res.x - 2)
		out_wave = noise2D(vec2(noise_scale * (3 - normalized_coords.y), t * tscale));
	else if (int(gl_FragCoord.y) <= 1)
		out_wave = noise2D(vec2(noise_scale * (4 - normalized_coords.x), t * tscale));
	else if (int(gl_FragCoord.y) >= rend_res.y - 2)
		out_wave = noise2D(vec2(noise_scale * (1 + normalized_coords.x), t * tscale));
	else {
		out_wave = texture(tex_waves, normalized_coords - 20 * veloc / (vecs_res + 1)).x;
		//out_wave = 0.9 * out_wave + 0.1 * prev_out_wave;
	}

	out_color *= 0.5 + 0.8 * out_wave;
}

