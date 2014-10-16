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

#include "MonteCarloUnit.h"

#include "Constants.h"

using namespace Luculentus;


MonteCarloUnit::MonteCarloUnit(const unsigned long randomSeed)
  : biUnitDistribution(-1.0f, 1.0f)
  , unitDistribution(0.0f, 1.0f)
  , longitudeDistribution(0.0f, static_cast<float>(pi * 2.0f))
  , latitudeDistribution(static_cast<float>(-pi * 0.5f),
      static_cast<float>(pi * 0.5f))
  , wavelengthDistribution(380.0f, 780.0f)
{
  randomEngine.seed(randomSeed);
}

Vector3 MonteCarloUnit::GetCosineDistributedHemisphereVector()
{
  // First, generate polar coordinates in a disk.
  float phi = GetLongitude();
  float rq = GetUnit();
  float r = std::sqrt(rq);

  // Calculate the direction based on the polar coordinates.
  // This distribution is cosine-weighted.
  Vector3 v =
  {
    std::cos(phi) * r,
    std::sin(phi) * r,
    std::sqrt(1.0f - rq),
  };

  return v;
}

Vector3 MonteCarloUnit::GetHemisphereVector()
{
  // First, generate polar coordinates in a hemisphere.
  float phi   = GetLongitude();
  float theta = GetLatitude();

  // Calculate the direction based on the polar coordinates.
  Vector3 v =
  {
    std::cos(phi) * std::sin(theta),
    std::sin(phi) * std::sin(theta),
    std::cos(theta),
  };

  return v;
}
