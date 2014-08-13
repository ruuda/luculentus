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

#pragma once

namespace Luculentus
{
  class EmissiveMaterial
  {
    public:
      
      /// Returns the light intensity at the specified wavelength (for
      /// emissive materials)
      virtual float GetIntensity(const float wavelength) const = 0;
  };

  class BlackBodyMaterial : public EmissiveMaterial
  {
    public:

      /// The temperature of the black body, in Kelvin. 6504 is a warm
      /// white, higher values are blue-ish, lower are red-ish.
      const float temperature;

      /// Bodies with lower temperature also have a lower intensity,
      /// but for the purposes of a light source, only the distribution
      /// is important, not the intensity, so the distribution must be
      /// normalised.
      const float normalisationFactor;

      /// Constructs a black body material with the specified
      /// temperature in Kelvin.
      BlackBodyMaterial(const float kelvins, const float intensity);

      virtual float GetIntensity(const float wavelength) const;
  };
}
