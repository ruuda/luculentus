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

#include <random>
#include "Vector3.h"

namespace Luculentus
{
  /// An entropy provider that can be kept per-thread.
  class MonteCarloUnit
  {
    public:

      /// Random generator.
      std::mt19937 randomEngine;

      /// Uniform distribution in the range -1 .. 1.
      std::uniform_real_distribution<float> biUnitDistribution;

      /// Uniform distribution in the range 0 .. 1.
      std::uniform_real_distribution<float> unitDistribution;

      /// Uniform distribution in the range 0 .. 2pi.
      std::uniform_real_distribution<float> longitudeDistribution;

      /// Uniform distribution in the range -pi/2 .. pi/2.
      std::uniform_real_distribution<float> latitudeDistribution;

      /// Uniform distribution in the range 380 .. 780.
      std::uniform_real_distribution<float> wavelengthDistribution;

      /// Initializes a new entropy provider with the specified seed.
      MonteCarloUnit(const long unsigned int seed);

      /// Returns a random real in the range -1 .. 1.
      inline float GetBiUnit()
      { return biUnitDistribution(randomEngine); }

      /// Returns a random real in the range 0 .. 1.
      inline float GetUnit()
      { return unitDistribution(randomEngine); }

      /// Returns a random real in the range 0 .. 2pi.
      inline float GetLongitude()
      { return longitudeDistribution(randomEngine); }

      /// Returns a random real in the range -pi/2 .. pi/2.
      inline float GetLatitude()
      { return latitudeDistribution(randomEngine); }

      /// Returns a random real in the range 380 .. 780.
      inline float GetWavelength()
      { return wavelengthDistribution(randomEngine); }

      /// Returns a random unit vector, pointing up along the z-axis,
      /// in the hemisphere bounded by the xy-plane, with a
      /// cosine-weighted probability.
      Vector3 GetCosineDistributedHemisphereVector();

      /// Returns a random unit vector, pointing up along the z-axis,
      /// in the hemisphere bounded by the xy-plane, with a uniform
      /// probability.
      Vector3 GetHemisphereVector();
  };
}
