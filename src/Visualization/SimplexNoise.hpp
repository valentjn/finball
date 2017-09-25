/// Copyright (c) 2014-2015 Sebastien Rombauts (sebastien.rombauts@gmail.com)
///	Modified 2017 by Sebastian Hasler (haslersn@fius.informatik.uni-stuttgart.de)
///
/// Distributed under the MIT License (MIT) (See copy at http://opensource.org/licenses/MIT)
#pragma once

namespace simplex
{

float noise1D(float x);
float noise2D(float x, float y);

float fractal1D(float frequency, float lacunarity, float persistence, int octaves, float x);
float fractal2D(float frequency, float lacunarity, float persistence, int octaves, float x, float y);

} // namespace simplex
