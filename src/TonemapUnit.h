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

#include <cstdint>
#include <vector>

namespace Luculentus
{
  class GatherUnit;

  /// Converts the result of a GatherUnit into an sRGB image.
  class TonemapUnit
  {
    public:

      /// Width of the canvas (in pixels).
      const int imageWidth;

      /// Height of the canvas (in pixels).
      const int imageHeight;

      /// The buffer of sRGB values.
      std::vector<std::uint8_t> rgbBuffer;

      /// Constructs a new tonemap unit that will tonemap a canvas
      /// of the specified size.
      TonemapUnit(const int width, const int height);

      /// Converts the unweighted CIE XYZ values in the GatherUnit
      /// to tonemapped sRGB values.
      void Tonemap(const GatherUnit& gatherUnit);

    private:

      /// Returns an exposure estimate based on the average cieY value.
      /// The returned value is the maximum acceptable intensity, the
      /// intensity that should become (nearly) white.
      float FindExposure(const GatherUnit& gatherUnit) const;
  };
}
