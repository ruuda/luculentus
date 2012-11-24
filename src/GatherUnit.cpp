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

#include "GatherUnit.h"

#include <memory>
#include "PlotUnit.h"

using namespace Luculentus;

GatherUnit::GatherUnit(const int width, const int height)
  : imageWidth(width)
  , imageHeight(height)
{
  // Allocate a buffer to store the tristimulus values
  tristimulusBuffer = new float[imageWidth * imageHeight * 3];

  // And make sure the buffer is black
  std::uninitialized_fill_n(tristimulusBuffer,
                            imageWidth * imageHeight * 3, 0.0f);
}

GatherUnit::~GatherUnit()
{
  delete [] tristimulusBuffer;
}

void GatherUnit::Accumulate(PlotUnit& plotUnit)
{
  // Loop through all pixels, and add the values
  for (int i = 0; i < imageWidth * imageHeight * 3; i++)
  {
    tristimulusBuffer[i] += plotUnit.tristimulusBuffer[i];
  }

  // Then clear the buffer of the plot unit, so it can be recycled
  plotUnit.Clear();
}
