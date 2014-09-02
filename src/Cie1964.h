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

#pragma once

#include "Vector3.h"

namespace Luculentus
{
  /// Helper class for the CIE 1931 colour space
  class Cie1964
  {
    private:

      /// CIE XYZ X tristimulus value, at 5nm intervals,
      /// starting at 380nm
      static const float x[81];

      /// CIE XYZ X tristimulus value, at 5nm intervals,
      /// starting at 380nm
      static const float y[81];

      /// CIE XYZ X tristimulus value, at 5nm intervals,
      /// starting at 380nm
      static const float z[81];

    public:
    
      static Vector3 GetTristimulus(float wavelength);
  };
}
