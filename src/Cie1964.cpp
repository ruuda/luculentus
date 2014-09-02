// Luculentus -- Proof of concept spectral path tracer
// Copyright (C) 2012-2014  Ruud van Asseldonk
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

#include "Cie1964.h"

#include <cmath>

using namespace Luculentus;

Vector3 Cie1964::GetTristimulus(float wavelength)
{
  float indexf    = (wavelength - 380.0f) / 5.0f;
  short index     = static_cast<short>(std::floor(indexf));
  float remainder = indexf - index;

  if (index < -1 || index > 80) // Wavelength is not in visible spectrum
  {
    return ZeroVector3();
  }

  if (index == -1) // No interpolation possible
  {
    float cieX = x[0] * (remainder);
    float cieY = y[0] * (remainder);
    float cieZ = z[0] * (remainder);
    return MakeVector3(cieX, cieY, cieZ);
  }

  if (index == 80) // No interpolation possible
  {
    float cieX = x[80] * (1.0f - remainder);
    float cieY = y[80] * (1.0f - remainder);
    float cieZ = z[80] * (1.0f - remainder);
    return MakeVector3(cieX, cieY, cieZ);
  }

  // Interpolate between two measurements
  float cieX = x[index] * (1.0f - remainder) + x[index + 1] * remainder;
  float cieY = y[index] * (1.0f - remainder) + y[index + 1] * remainder;
  float cieZ = z[index] * (1.0f - remainder) + z[index + 1] * remainder;
  return MakeVector3(cieX, cieY, cieZ);
}

// Data obtained from http://cvrl.ioo.ucl.ac.uk/index.htm

const float Cie1964::x[81] =
{
  0.000160f,
  0.000662f,
  0.002362f,
  0.007242f,
  0.019110f,
  0.043400f,
  0.084736f,
  0.140638f,
  0.204492f,
  0.264737f,
  0.314679f,
  0.357719f,
  0.383734f,
  0.386726f,
  0.370702f,
  0.342957f,
  0.302273f,
  0.254085f,
  0.195618f,
  0.132349f,
  0.080507f,
  0.041072f,
  0.016172f,
  0.005132f,
  0.003816f,
  0.015444f,
  0.037465f,
  0.071358f,
  0.117749f,
  0.172953f,
  0.236491f,
  0.304213f,
  0.376772f,
  0.451584f,
  0.529826f,
  0.616053f,
  0.705224f,
  0.793832f,
  0.878655f,
  0.951162f,
  1.014160f,
  1.074300f,
  1.118520f,
  1.134300f,
  1.123990f,
  1.089100f,
  1.030480f,
  0.950740f,
  0.856297f,
  0.754930f,
  0.647467f,
  0.535110f,
  0.431567f,
  0.343690f,
  0.268329f,
  0.204300f,
  0.152568f,
  0.112210f,
  0.081261f,
  0.057930f,
  0.040851f,
  0.028623f,
  0.019941f,
  0.013842f,
  0.009577f,
  0.006605f,
  0.004553f,
  0.003145f,
  0.002175f,
  0.001506f,
  0.001045f,
  0.000727f,
  0.000508f,
  0.000356f,
  0.000251f,
  0.000178f,
  0.000126f,
  0.000090f,
  0.000065f,
  0.000046f,
  0.000033f
};

const float Cie1964::y[81] =
{
  0.000017f,
  0.000072f,
  0.000253f,
  0.000769f,
  0.002004f,
  0.004509f,
  0.008756f,
  0.014456f,
  0.021391f,
  0.029497f,
  0.038676f,
  0.049602f,
  0.062077f,
  0.074704f,
  0.089456f,
  0.106256f,
  0.128201f,
  0.152761f,
  0.185190f,
  0.219940f,
  0.253589f,
  0.297665f,
  0.339133f,
  0.395379f,
  0.460777f,
  0.531360f,
  0.606741f,
  0.685660f,
  0.761757f,
  0.823330f,
  0.875211f,
  0.923810f,
  0.961988f,
  0.982200f,
  0.991761f,
  0.999110f,
  0.997340f,
  0.982380f,
  0.955552f,
  0.915175f,
  0.868934f,
  0.825623f,
  0.777405f,
  0.720353f,
  0.658341f,
  0.593878f,
  0.527963f,
  0.461834f,
  0.398057f,
  0.339554f,
  0.283493f,
  0.228254f,
  0.179828f,
  0.140211f,
  0.107633f,
  0.081187f,
  0.060281f,
  0.044096f,
  0.031800f,
  0.022602f,
  0.015905f,
  0.011130f,
  0.007749f,
  0.005375f,
  0.003718f,
  0.002565f,
  0.001768f,
  0.001222f,
  0.000846f,
  0.000586f,
  0.000407f,
  0.000284f,
  0.000199f,
  0.000140f,
  0.000098f,
  0.000070f,
  0.000050f,
  0.000036f,
  0.000025f,
  0.000018f,
  0.000013f
};

const float Cie1964::z[81] =
{
  0.000705f,
  0.002928f,
  0.010482f,
  0.032344f,
  0.086011f,
  0.197120f,
  0.389366f,
  0.656760f,
  0.972542f,
  1.282500f,
  1.553480f,
  1.798500f,
  1.967280f,
  2.027300f,
  1.994800f,
  1.900700f,
  1.745370f,
  1.554900f,
  1.317560f,
  1.030200f,
  0.772125f,
  0.570060f,
  0.415254f,
  0.302356f,
  0.218502f,
  0.159249f,
  0.112044f,
  0.082248f,
  0.060709f,
  0.043050f,
  0.030451f,
  0.020584f,
  0.013676f,
  0.007918f,
  0.003988f,
  0.001091f,
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
