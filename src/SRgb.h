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

#pragma once
#include <cmath>

namespace Luculentus
{
  /// Helper class for the sRGB colour space
  class SRgb
  {
    private:

      /// Applies the sRGB gamma correction to the component
      inline static float GammaCorrect(float f)
      {
        if (f <= 0.0031308f)
        {
          return 12.92f * f;
        }
        else
        {
          return 1.055f * std::pow(f, 1.0f / 2.4f) - 0.055f;
        }
      }

    public:

      /// Converts a CIE XYZ tristimulus to an sRGB colour
      static void Transform(float cieX, float cieY, float cieZ,
                            float& r, float& g, float& b);
  };
}
