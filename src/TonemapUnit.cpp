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

#include "TonemapUnit.h"

#include <cmath>
#include <algorithm>
#include "GatherUnit.h"
#include "SRgb.h"

using namespace Luculentus;

TonemapUnit::TonemapUnit(const int width, const int height)
  : imageWidth(width)
  , imageHeight(height)
{
  // Allocate a buffer to store the sRGB values
  rgbBuffer = new std::uint8_t[imageWidth * imageHeight * 3];
}

TonemapUnit::~TonemapUnit()
{
  delete [] rgbBuffer;
}

void TonemapUnit::Tonemap(const GatherUnit& gatherUnit)
{
  float maxIntensity = FindExposure(gatherUnit);

  for (int x = 0; x < imageWidth; x++)
  {
    for (int y = 0; y < imageHeight; y++)
    {
      float cieX = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 0];
      float cieY = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 1];
      float cieZ = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 2];

      // Apply exposure correction
      cieX = std::log(cieX / maxIntensity + 1.0f) / std::log(4.0f);
      cieY = std::log(cieY / maxIntensity + 1.0f) / std::log(4.0f);
      cieZ = std::log(cieZ / maxIntensity + 1.0f) / std::log(4.0f);

      // Convert to sRGB
      float r, g, b;
      SRgb::Transform(cieX, cieY, cieZ, r, g, b);

      // Clamp colours to saturate
      r = std::min<float>(1.0f, std::max<float>(0.0f, r));
      g = std::min<float>(1.0f, std::max<float>(0.0f, g));
      b = std::min<float>(1.0f, std::max<float>(0.0f, b));

      // Convert to integers
      rgbBuffer[y * imageWidth * 3 + x * 3 + 0] = static_cast<std::uint8_t>(std::min<int>(static_cast<int>(r * 255), 255));
      rgbBuffer[y * imageWidth * 3 + x * 3 + 1] = static_cast<std::uint8_t>(std::min<int>(static_cast<int>(g * 255), 255));
      rgbBuffer[y * imageWidth * 3 + x * 3 + 2] = static_cast<std::uint8_t>(std::min<int>(static_cast<int>(b * 255), 255));
    }
  }
}

float TonemapUnit::FindExposure(const GatherUnit& gatherUnit) const
{
  // Calculate the average intensity
  float sum = 0.0f;
  for (int x = 0; x < imageWidth; x++)
  {
    float intermediateSum = 0.0f;
    for (int y = 0; y < imageHeight; y++)
    {
      // Calculations are based on the CIE Y value (corresponds to lightness),
      // but X and Z are also into account slightly to avoid weird situations
      float cieX = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 0];
      float cieY = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 1];
      float cieZ = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 2];

      float intensity = (cieY * 5.0f + cieX + cieZ) * (1.0f / 7.0f);
      intermediateSum += intensity;
    }
    sum += intermediateSum / imageHeight;
  }

  float average = sum / imageWidth;

  // Then calculate the standard deviation
  sum = 0.0f;
  for (int x = 0; x < imageWidth; x++)
  {
    float intermediateSum = 0.0f;
    for (int y = 0; y < imageHeight; y++)
    {
      float cieX = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 0];
      float cieY = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 1];
      float cieZ = gatherUnit.tristimulusBuffer[y * imageWidth * 3 + x * 3 + 2];

      float intensity = (cieY * 5.0f + cieX + cieZ) * (1.0f / 7.0f);
      intermediateSum += (intensity - average) * (intensity - average);
    }
    sum += intermediateSum / imageHeight;
  }

  float variance = sum / imageWidth;
  float standardDeviation = std::sqrt(variance);

  // The desired 'white' is one standard deviation above average
  float maxIntensity = average + standardDeviation;
  return maxIntensity;
}
