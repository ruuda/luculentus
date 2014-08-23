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

#include "PlotUnit.h"

#include <algorithm>
#include "TraceUnit.h"
#include "Cie1931.h"

using namespace Luculentus;

PlotUnit::PlotUnit(const int width, const int height)
  : imageWidth(width)
  , imageHeight(height)
  , aspectRatio(static_cast<float>(width) / static_cast<float>(height))
{
  // Allocate a buffer to store the tristimulus values,
  // and fill it with black
  tristimulusBuffer.resize(imageWidth * imageHeight, ZeroVector3());
}

void PlotUnit::Clear()
{
  std::fill(tristimulusBuffer.begin(), tristimulusBuffer.end(), ZeroVector3());
}

void PlotUnit::Plot(const TraceUnit& traceUnit)
{
  // Loop trough every mapped photon, and plot it.
  for (auto photon : traceUnit.mappedPhotons)
  {
    // Calculate the CIE tristimulus values, given the wavelength.
    Vector3 cie = Cie1931::GetTristimulus(photon.wavelength);

    // Then plot the pixel into the buffer.
    PlotPixel(photon.x, photon.y, cie * photon.probability);
  }
}

void PlotUnit::PlotPixel(float x, float y, Vector3 cie)
{
  // Map the position to some pixels
  float px = (x * 0.5f + 0.5f) * (imageWidth - 1);
  float py = (y * aspectRatio * 0.5f + 0.5f) * (imageHeight - 1);
    
  // Map to discrete pixels
  int px1 = std::max(0, std::min(imageWidth - 1,
                          static_cast<int>(std::floor(px))));
  int px2 = std::max(0, std::min(imageWidth - 1, 
                          static_cast<int>(std::ceil(px))));
  int py1 = std::max(0, std::min(imageHeight - 1, 
                          static_cast<int>(std::floor(py))));
  int py2 = std::max(0, std::min(imageHeight - 1, 
                          static_cast<int>(std::ceil(py))));

  // Calculate pixel coefficients
  float cx = px - px1;
  float cy = py - py1;
  float c11 = (1.0f - cx) * (1.0f - cy);
  float c12 = (1.0f - cx) * cy;
  float c21 = cx * (1.0f - cy);
  float c22 = cx * cy;

  // Plot the four pixels
  tristimulusBuffer[py1 * imageWidth + px1] += cie * c11;
  tristimulusBuffer[py1 * imageWidth + px2] += cie * c21;
  tristimulusBuffer[py2 * imageWidth + px1] += cie * c12;
  tristimulusBuffer[py2 * imageWidth + px2] += cie * c22;
}
