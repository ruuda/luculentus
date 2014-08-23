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
      float r = rgb.x;
      float g = rgb.y;
      float b = rgb.z;

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
      // Calculations are based on the CIE Y value,
      // which corresponds to lightness.
      Vector3 cie = gatherUnit.tristimulusBuffer[y * imageWidth + x];
      intermediateSum += cie.y;
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
      Vector3 cie = gatherUnit.tristimulusBuffer[y * imageWidth + x];
      intermediateSum += cie.y * cie.y;
    }
    sum += intermediateSum / imageHeight;
  }

  float averageSqr = sum / imageWidth;
  float variance = averageSqr - average * average;
  float standardDeviation = std::sqrt(variance);

  // The desired 'white' is one standard deviation above average
  float maxIntensity = average + standardDeviation;
  return maxIntensity;
}
