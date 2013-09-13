// Luculentus -- Proof of concept spectral path tracer
// Copyright (C) 2012  Ruud van Asseldonk
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Cie1931.h"

#include <cmath>

using namespace Luculentus;

void Cie1931::GetTristimulus(float wavelength, float& cieX, float& cieY,
                             float& cieZ)
{
  float indexf    = (wavelength - 380.0f) / 5.0f;
  short index     = static_cast<short>(std::floor(indexf));
  float remainder = indexf - index;

  if (index < -1 || index > 80) // Wavelength is not in visible spectrum
  {
    cieX = 0.0f;
    cieY = 0.0f;
    cieZ = 0.0f;
    return;
  }

  if (index == -1) // No interpolation possible
  {
    cieX = x[0] * (remainder);
    cieY = y[0] * (remainder);
    cieZ = z[0] * (remainder);
    return;
  }

  if (index == 80) // No interpolation possible
  {
    cieX = x[80] * (1.0f - remainder);
    cieY = y[80] * (1.0f - remainder);
    cieZ = z[80] * (1.0f - remainder);
    return;
  }

  // Interpolate between two measurements
  cieX = x[index] * (1.0f - remainder) + x[index + 1] * remainder;
  cieY = y[index] * (1.0f - remainder) + y[index + 1] * remainder;
  cieZ = z[index] * (1.0f - remainder) + z[index + 1] * remainder;
}

// Data obtained from http://cvrl.ioo.ucl.ac.uk/index.htm

const float Cie1931::x[81] =
{
  0.001368f,
  0.002236f,
  0.004243f,
  0.007650f,
  0.014310f,
  0.023190f,
  0.043510f,
  0.077630f,
  0.134380f,
  0.214770f,
  0.283900f,
  0.328500f,
  0.348280f,
  0.348060f,
  0.336200f,
  0.318700f,
  0.290800f,
  0.251100f,
  0.195360f,
  0.142100f,
  0.095640f,
  0.057950f,
  0.032010f,
  0.014700f,
  0.004900f,
  0.002400f,
  0.009300f,
  0.029100f,
  0.063270f,
  0.109600f,
  0.165500f,
  0.225750f,
  0.290400f,
  0.359700f,
  0.433450f,
  0.512050f,
  0.594500f,
  0.678400f,
  0.762100f,
  0.842500f,
  0.916300f,
  0.978600f,
  1.026300f,
  1.056700f,
  1.062200f,
  1.045600f,
  1.002600f,
  0.938400f,
  0.854450f,
  0.751400f,
  0.642400f,
  0.541900f,
  0.447900f,
  0.360800f,
  0.283500f,
  0.218700f,
  0.164900f,
  0.121200f,
  0.087400f,
  0.063600f,
  0.046770f,
  0.032900f,
  0.022700f,
  0.015840f,
  0.011359f,
  0.008111f,
  0.005790f,
  0.004109f,
  0.002899f,
  0.002049f,
  0.001440f,
  0.001000f,
  0.000690f,
  0.000476f,
  0.000332f,
  0.000235f,
  0.000166f,
  0.000117f,
  0.000083f,
  0.000059f,
  0.000042f
};

const float Cie1931::y[81] =
{
  0.000039f,
  0.000064f,
  0.000120f,
  0.000217f,
  0.000396f,
  0.000640f,
  0.001210f,
  0.002180f,
  0.004000f,
  0.007300f,
  0.011600f,
  0.016840f,
  0.023000f,
  0.029800f,
  0.038000f,
  0.048000f,
  0.060000f,
  0.073900f,
  0.090980f,
  0.112600f,
  0.139020f,
  0.169300f,
  0.208020f,
  0.258600f,
  0.323000f,
  0.407300f,
  0.503000f,
  0.608200f,
  0.710000f,
  0.793200f,
  0.862000f,
  0.914850f,
  0.954000f,
  0.980300f,
  0.994950f,
  1.000000f,
  0.995000f,
  0.978600f,
  0.952000f,
  0.915400f,
  0.870000f,
  0.816300f,
  0.757000f,
  0.694900f,
  0.631000f,
  0.566800f,
  0.503000f,
  0.441200f,
  0.381000f,
  0.321000f,
  0.265000f,
  0.217000f,
  0.175000f,
  0.138200f,
  0.107000f,
  0.081600f,
  0.061000f,
  0.044580f,
  0.032000f,
  0.023200f,
  0.017000f,
  0.011920f,
  0.008210f,
  0.005723f,
  0.004102f,
  0.002929f,
  0.002091f,
  0.001484f,
  0.001047f,
  0.000740f,
  0.000520f,
  0.000361f,
  0.000249f,
  0.000172f,
  0.000120f,
  0.000085f,
  0.000060f,
  0.000042f,
  0.000030f,
  0.000021f,
  0.000015f
};

const float Cie1931::z[81] =
{
  0.006450f,
  0.010550f,
  0.020050f,
  0.036210f,
  0.067850f,
  0.110200f,
  0.207400f,
  0.371300f,
  0.645600f,
  1.039050f,
  1.385600f,
  1.622960f,
  1.747060f,
  1.782600f,
  1.772110f,
  1.744100f,
  1.669200f,
  1.528100f,
  1.287640f,
  1.041900f,
  0.812950f,
  0.616200f,
  0.465180f,
  0.353300f,
  0.272000f,
  0.212300f,
  0.158200f,
  0.111700f,
  0.078250f,
  0.057250f,
  0.042160f,
  0.029840f,
  0.020300f,
  0.013400f,
  0.008750f,
  0.005750f,
  0.003900f,
  0.002750f,
  0.002100f,
  0.001800f,
  0.001650f,
  0.001400f,
  0.001100f,
  0.001000f,
  0.000800f,
  0.000600f,
  0.000340f,
  0.000240f,
  0.000190f,
  0.000100f,
  0.000050f,
  0.000030f,
  0.000020f,
  0.000010f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f,
  0.000000f
};
