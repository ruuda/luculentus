// Luculentus -- Proof of concept spectral path tracer
// Copyright (C) 2012, 2014  Ruud van Asseldonk
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

#include "SRgb.h"

using namespace Luculentus;

Vector3 SRgb::Transform(Vector3 cie)
{
  // Apply sRGB matrix
  float r =  3.2406f * cie.x - 1.5372f * cie.y - 0.4986f * cie.z;
  float g = -0.9689f * cie.x + 1.8758f * cie.y + 0.0415f * cie.z;
  float b =  0.0557f * cie.x - 0.2040f * cie.y + 1.0570f * cie.z;

  // Then do gamma correction
  Vector3 rgb =
  {
    GammaCorrect(r),
    GammaCorrect(g),
    GammaCorrect(b)
  };

  return rgb;
}
