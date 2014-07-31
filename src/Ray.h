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
#include "Vector3.h"

namespace Luculentus
{
  struct Ray
  {
    /// The 'position' of the ray
    Vector3 origin;

    /// The normalised direction in which the ray is pointing
    Vector3 direction;

    /// The wavelength of the light ray
    /// (in nm, so in the range 380 - 780)
    float wavelength;

    /// The probability that a photon followed this light path.
    /// Note that this can also be compensated for,
    /// if the probability of the ray being generated is not uniform.
    float probability;
  };
}
