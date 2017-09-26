#version 330
#extension GL_ARB_explicit_uniform_location : require

uniform sampler2D tex_vecs; // velocities texture
uniform sampler2D tex_noise; // noise texture
uniform sampler2D tex_waves; // density texture
uniform uint t;

layout(location = 0) out vec3 out_color;
layout(location = 1) out float out_wave;

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
	return (grad * x);				// Multiply the gradient with the distance
}

float grad(int hash, vec2 uv)
{
	int h = hash & 0x3F;		// Convert low 3 bits of hash code
	if (h >= 4) uv = uv.yx;
	if ((h & 1) != 0) uv.x = -uv.x;
	if ((h & 2) != 0) uv.y = -uv.y;
	return dot(uv, vec2(1, 2));
}

float noise2D(vec2 coords)
{
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
	ivec2 i1j1 = ivec2(0, 1);
	if (x0y0.x > x0y0.y)
		i1j1 = i1j1.yx;

	vec2 x1y1 = x0y0 - i1j1 + G2;
	vec2 x2y2 = x0y0 - 1.0 + 2.0 * G2;

	// Calculate the contributions
	vec3 n;
	n.x = dot(x0y0, x0y0);
	n.y = dot(x1y1, x1y1);
	n.z = dot(x2y2, x2y2);
	n = 0.5 - n;
	n = max(n, 0.0);
	if (n.x != 0.0) {
		n.x *= n.x;
		n.x *= n.x;
		n.x *= grad(hash(ij.x + hash(ij.y)), x0y0);
	}
	if (n.y != 0.0) {
		n.y *= n.y;
		n.y *= n.y;
		n.y *= grad(hash(ij.x + i1j1.x + hash(ij.y + i1j1.y)), x1y1);
	}
	if (n.z != 0.0) {
		n.z *= n.z;
		n.z *= n.z;
		n.z *= grad(hash(ij.x + 1 + hash(ij.y + 1)), x2y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 45.23065 * dot(n, vec3(1));
}

vec2 getVeloc(vec2 coords) {
	return (texture(tex_vecs, coords).xy - 0.5) * 0.1;
}

// perfrom line integral convolution
float lic(vec2 normalized_coords)
{
	const int lic_steps = 20; // The number of adjacent locations in one direction to use for smearing
	const int velocity_scale = 20;

	vec2 texel_size = 1.0 / textureSize(tex_noise, 0);
	float running_total = 0.0; // running total of grey-scale texture values

	// Start at this fragment's location
	vec2 step_coords = normalized_coords;
	
	// step FORWARD along the vector field
	for (int i = 0; i < lic_steps; ++i) {
		vec2 v = getVeloc(step_coords) * velocity_scale;
		step_coords += v * texel_size;
		running_total += texture(tex_noise, step_coords).x;
	}

	// resart at this fragment's location
	step_coords = normalized_coords;

	// step BACKWARD along the vector field
	for (int i = 0; i < lic_steps; ++i) {
		vec2 v = getVeloc(step_coords) * velocity_scale;
		step_coords -= v * texel_size;
		running_total += texture(tex_noise, step_coords).x;
	}

	return running_total / (lic_steps * 2);
}

void main()
{
    const float noise_scale = 2;
	const float tscale = 0.1;

	ivec2 vecs_res = textureSize(tex_vecs, 0);
	ivec2 rend_res = textureSize(tex_noise, 0) * 2;
    vec2 normalized_coords = gl_FragCoord.xy / rend_res;
    //normalized_coords = (normalized_coords * vecs_res + 0.5) / (vecs_res + 1);

    /* float density_val = 2 * texture(tex_vecs, normalized_coords).z;
    if (density_val < 1)
		out_color = 2 * vec3(0, 0, 1) * (1 - density_val) + vec3(0, 1, 0) * density_val;
	else {
        density_val -= 1;
        density_val = min(density_val, 1);
		out_color = vec3(0, 1, 0) * (1 - density_val) + 2 * vec3(1, 0, 0) * density_val;
	}*/
	
	vec2 veloc = getVeloc(normalized_coords);
	float velocity_val = lic(normalized_coords);
	if (int(gl_FragCoord.x) <= 1)
		out_wave = noise2D(vec2(noise_scale * normalized_coords.y, t * tscale));
	else if (int(gl_FragCoord.x) >= rend_res.x - 2)
		out_wave = noise2D(vec2(noise_scale * (3 - normalized_coords.y), t * tscale));
	else if (int(gl_FragCoord.y) <= 1)
		out_wave = noise2D(vec2(noise_scale * (4 - normalized_coords.x), t * tscale));
	else if (int(gl_FragCoord.y) >= rend_res.y - 2)
		out_wave = noise2D(vec2(noise_scale * (1 + normalized_coords.x), t * tscale));
	else {
		vec2 sample_coords = normalized_coords - 10 * veloc / vecs_res;
		out_wave = texture(tex_waves, sample_coords).x;
		velocity_val *= 1.0 + 0.4 * out_wave;
	}
	float vec_mul = min(length(veloc) * 32, 1);
	vec3 color = vec3(0.4, 0.5, 1);
	vec3 color_calc = min(color * velocity_val, 1);
	vec3 color_def = 0.5 * color;
	out_color = color_calc * vec_mul + color_def * (1 - vec_mul);
}
