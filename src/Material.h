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

#include "Ray.h"
#include "Intersection.h"

namespace Luculentus
{
  class MonteCarloUnit;

  class Material
  {
    public:

      /// Returns the ray that continues the light path, backwards
      /// from the camera to the light source.
      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const = 0;
  };

  /// A perfectly diffuse, perfectly reflecting all wavelengths, material
  class ClayMaterial : public Material
  {
    public:

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;
  };

  /// Same as clay, but not perfectly white; it absorbes energy
  class DiffuseGreyMaterial : public ClayMaterial
  {
    public:

      /// How much the material reflects; 0.0 is black, 1.0 is white (clay material)
      const float reflectance;

      /// Constructs a new diffuse gray material with the specified reflectance
      DiffuseGreyMaterial(const float refl);

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;
  };

  /// Reflects light of a certain wavelength better than others, with a normal distribution
  class DiffuseColouredMaterial : public DiffuseGreyMaterial
  {
    public: 

      /// The wavelength that is best reflected
      const float wavelength;

      /// The standard deviation for the distribution
      const float deviation;

      DiffuseColouredMaterial(const float refl, const float wavel, const float dev);

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;
  };

  /// Reflects all light perfectly along the same (but opposite) angle
  class PerfectMirrorMaterial : public Material
  {
    public:

      virtual Ray GetNewRay(const Ray incomingRay,
                      const Intersection intersection,
                      MonteCarloUnit& monteCarloUnit) const;
  };

  /// Blends between perfect reflection and diffuse
  class GlossyMirrorMaterial : public Material
  {
    public:

      /// The amount of 'gloss', where 1.0 equals diffuse,
      /// and 0.0 is a perfect mirror.
      const float glossiness;

      GlossyMirrorMaterial(const float gloss);

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;
  };

  class BrushedMetalMaterial : public Material
  {
    public:

      /// The amount of 'gloss', where 1.0 equals diffuse,
      /// and 0.0 is a perfect mirror.
      const float glossiness;

      /// The amount of directional dependence of the scattering, where
      /// 0.0 is a normal mirror, and 1.0 is diffuse along the tangent.
      const float anisotropy;

      BrushedMetalMaterial(const float gloss, const float aniso);

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;
  };

  class RefractiveMaterial : public Material
  {
    public:

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;

      virtual float GetIndexOfRefraction(const float wavelength) const = 0;
  };

  class Bk7GlassMaterial : public RefractiveMaterial
  {
    public:

      virtual float GetIndexOfRefraction(const float wavelength) const;
  };

  class Sf10GlassMaterial : public RefractiveMaterial
  {
    public:

      virtual float GetIndexOfRefraction(const float wavelength) const;
  };

  /// A not physically accurate, but still aesthetically pleasant soap
  /// material.
  class SoapBubbleMaterial : public Material
  {
    public:

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;
  };

  /// A completely fictional but very interesting
  class IridescentMaterial : public Material
  {
    public:

      virtual Ray GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const;
  };
}
