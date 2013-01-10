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

#include "PlotUnit.h"

#include <memory>
#include "TraceUnit.h"
#include "Cie1931.h"

using namespace Luculentus;

PlotUnit::PlotUnit(const int width, const int height)
  : imageWidth(width)
  , imageHeight(height)
  , aspectRatio(static_cast<float>(width) / static_cast<float>(height))
{
  // Allocate a buffer to store the tristimulus values
  tristimulusBuffer = new float[imageWidth * imageHeight * 3];

  // And make sure the buffer is black
  Clear();
}

PlotUnit::~PlotUnit()
{
  delete [] tristimulusBuffer;
}

void PlotUnit::Clear()
{
  std::uninitialized_fill_n(tristimulusBuffer, 
    imageWidth * imageHeight * 3, 0.0f);
}

void PlotUnit::Plot(const TraceUnit& traceUnit)
{
  // Loop trough every mapped photon, and plot it.
  for (int i = 0; i < traceUnit.numberOfMappedPhotons; i++)
  {
    const MappedPhoton& mp = traceUnit.mappedPhotons[i];

    PlotPhoton(mp.x, mp.y, mp.wavelength, mp.probability);
  }
}

void PlotUnit::PlotPhoton(float x, float y, float wavelength,
                          float intensity)
{
  // Calculate the CIE tristimulus values, given the wavelength
  float cieX, cieY, cieZ;
  Cie1931::GetTristimulus(wavelength, cieX, cieY, cieZ);

  cieX *= intensity;
  cieY *= intensity;
  cieZ *= intensity;

  // Then plot the pixel into the buffer
  PlotPixel(x, y, cieX, cieY, cieZ);
}

void PlotUnit::PlotPixel(float x, float y, float cieX, float cieY,
                         float cieZ)
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
  tristimulusBuffer[py1 * imageWidth * 3 + px1 * 3 + 0] += cieX * c11;
  tristimulusBuffer[py1 * imageWidth * 3 + px1 * 3 + 1] += cieY * c11;
  tristimulusBuffer[py1 * imageWidth * 3 + px1 * 3 + 2] += cieZ * c11;

  tristimulusBuffer[py1 * imageWidth * 3 + px2 * 3 + 0] += cieX * c21;
  tristimulusBuffer[py1 * imageWidth * 3 + px2 * 3 + 1] += cieY * c21;
  tristimulusBuffer[py1 * imageWidth * 3 + px2 * 3 + 2] += cieZ * c21;

  tristimulusBuffer[py2 * imageWidth * 3 + px1 * 3 + 0] += cieX * c12;
  tristimulusBuffer[py2 * imageWidth * 3 + px1 * 3 + 1] += cieY * c12;
  tristimulusBuffer[py2 * imageWidth * 3 + px1 * 3 + 2] += cieZ * c12;

  tristimulusBuffer[py2 * imageWidth * 3 + px2 * 3 + 0] += cieX * c22;
  tristimulusBuffer[py2 * imageWidth * 3 + px2 * 3 + 1] += cieY * c22;
  tristimulusBuffer[py2 * imageWidth * 3 + px2 * 3 + 2] += cieZ * c22;
}