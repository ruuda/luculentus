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
  class TraceUnit;

  /// Handles plotting the results of a TraceUnit.
  class PlotUnit
  {
    public:

      /// Width of the canvas (in pixels).
      const int imageWidth;

      /// Height of the canvas (in pixels).
      const int imageHeight;

      /// Width of the canvas divided by its height.
      const float aspectRatio;

      /// The buffer of tristimulus values.
      std::vector<Vector3> tristimulusBuffer;

      /// Constructs a new plot unit that will plot to a canvas
      /// of the specified size.
      PlotUnit(const int width, const int height);

      /// Plots the result of the specified TraceUnit onto the canvas.
      void Plot(const TraceUnit& traceUnit);

      /// Resets the tristimulus buffer.
      void Clear();

    private:

      /// Plots a pixel, anti-aliased into the buffer
      /// (adding it to existing content).
      void PlotPixel(float x, float y, Vector3 cie);
  };
}
