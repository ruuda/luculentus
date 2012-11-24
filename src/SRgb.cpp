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

#include "SRgb.h"

using namespace Luculentus;

void SRgb::Transform(float cieX, float cieY, float cieZ, float& r,
                     float& g, float& b)
{
  // Apply sRGB matrix
  r =  3.2406f * cieX - 1.5372f * cieY - 0.4986f * cieZ;
  g = -0.9689f * cieX + 1.8758f * cieY + 0.0415f * cieZ;
  b =  0.0557f * cieX - 0.2040f * cieY + 1.0570f * cieZ;

  // Then do gamma correction
  r = GammaCorrect(r);
  g = GammaCorrect(g);
  b = GammaCorrect(b);
}
