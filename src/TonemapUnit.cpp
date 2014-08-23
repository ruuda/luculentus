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

#include "TonemapUnit.h"

#include <cmath>
#include <algorithm>
#include <numeric>
#include "GatherUnit.h"
#include "SRgb.h"

using namespace Luculentus;

TonemapUnit::TonemapUnit(const int width, const int height)
  : imageWidth(width)
  , imageHeight(height)
{
  // Allocate a buffer to store the sRGB values
  rgbBuffer.resize(imageWidth * imageHeight * 3, 0);
}

float clamp(const float x)
{
  return std::min(1.0f, std::max(0.0f, x));
}

void TonemapUnit::Tonemap(const GatherUnit& gatherUnit)
{
  float maxIntensity = FindExposure(gatherUnit);

  for (int x = 0; x < imageWidth; x++)
  {
    for (int y = 0; y < imageHeight; y++)
    {
      Vector3 cie = gatherUnit.tristimulusBuffer[y * imageWidth + x];

      // Apply exposure correction
      cie.x = std::log(cie.x / maxIntensity + 1.0f) / std::log(4.0f);
      cie.y = std::log(cie.y / maxIntensity + 1.0f) / std::log(4.0f);
      cie.z = std::log(cie.z / maxIntensity + 1.0f) / std::log(4.0f);

      // Convert to sRGB.
      Vector3 rgb = SRgb::Transform(cie);

      // Clamp colours to saturate.
      float r = clamp(rgb.x);
      float g = clamp(rgb.y);
      float b = clamp(rgb.z);

      // Convert to integers
      rgbBuffer[y * imageWidth * 3 + x * 3 + 0] = static_cast<std::uint8_t>(r * 255);
      rgbBuffer[y * imageWidth * 3 + x * 3 + 1] = static_cast<std::uint8_t>(g * 255);
      rgbBuffer[y * imageWidth * 3 + x * 3 + 2] = static_cast<std::uint8_t>(b * 255);
    }
  }
}

float TonemapUnit::FindExposure(const GatherUnit& gatherUnit) const
{
  float n = static_cast<float>(imageWidth * imageHeight);
  auto& tristimuli = gatherUnit.tristimulusBuffer;

  // Calculate the average intensity. Calculations are based
  // on the CIE Y component, which corresponds to lightness.
  float mean = std::accumulate(tristimuli.begin(), tristimuli.end(), 0.0f,
               [](float a, Vector3 cie) { return a + cie.y; }) / n;

  // Then compute the standard deviation.
  float sqrMean = std::accumulate(tristimuli.begin(), tristimuli.end(), 0.0f,
                  [](float a, Vector3 cie) { return a + cie.y * cie.y; }) / n;

  float variance = sqrMean - mean * mean;

  // The desired 'white' is one standard deviation above average
  return mean + std::sqrt(variance);
}
