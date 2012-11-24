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

#include "EmissiveMaterial.h"

#include <cmath>
#include "Constants.h"

using namespace Luculentus;

float boltzmann(const float wavelength, const float temperature)
{
  // Use double precision here, the numers are quite large/small,
  // might cause trouble
  const double h = plancksConstant;
  const double k = boltzmannsConstant;
  const double c = speedOfLight; // Speed of light in vacuum

  // Multiply by 1e-9 (nano), because wavelength is specified in nm,
  // while m is the standard unit.
  const double f = c / (wavelength * 1.0e-9);

  // The Boltzmann distribution
  return static_cast<float>((2.0 * h * f * f * f)
    / (c * c * (std::exp(h * f / (k * temperature)) - 1.0)));
}

BlackBodyMaterial::BlackBodyMaterial(const float kelvins,
                                     const float intensity)
  : temperature(kelvins)
  , normalisationFactor(intensity
    / boltzmann(static_cast<float>((wiensConstant / kelvins) * 1.0e9),
    kelvins))
{

}

float BlackBodyMaterial::GetIntensity(const float wavelength) const
{
  // The intensity for a specified wavelength,
  // is the normalised Boltzmann distribution
  return boltzmann(wavelength, temperature) * normalisationFactor;
}
