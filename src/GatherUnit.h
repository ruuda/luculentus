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

#include <vector>
#include "Vector3.h"

namespace Luculentus
{
  class PlotUnit;

  /// Handles combining the results of multiple PlotUnits
  class GatherUnit
  {
    public:

      /// Width of the canvas (in pixels).
      const int imageWidth;

      /// Height of the canvas (in pixels).
      const int imageHeight;

      /// The buffer of tristimulus values.
      std::vector<Vector3> tristimulusBuffer;

      /// Constructs a new gather unit that will gather a canvas of the
      /// specified size.
      GatherUnit(const int width, const int height);

      /// Add the results of the PlotUnit to the canvas,
      /// and then clears the PlotUnit, so it can be recycled.
      void Accumulate(PlotUnit& plotUnit);
  };
}
