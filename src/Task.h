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

#include <vector>

namespace Luculentus
{
  struct Task
  {
    enum TaskType
    {
      /// Do nothing, wait a while
      Sleep,
      /// Trace a certain number of rays, and store MappedPhotons.
      Trace,
      /// Plot all intermediate MappedPhotons
      /// to a screen of CIE XYZ values.
      Plot,
      /// Combine all CIE XYZ screens,
      /// and accumulate them into the final image.
      Gather,
      /// Convert and tonemap the CIE XYZ values
      /// to sRGB and display the image.
      Tonemap
    }
    /// The type of thing that should be done.
    type;

    /// The index of the unit to use to execute the task (for trace and plot tasks).
    int unit;

    /// The units that should be processed, e.g. for a Plot task, this
    /// contains the indices of the TraceUnits that must be plotted.
    std::vector<int> otherUnits;
  };
}
