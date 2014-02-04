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

#include "Material.h"

#include "MonteCarloUnit.h"
#include "Constants.h"

using namespace Luculentus;

Ray ClayMaterial::GetNewRay(const Ray incomingRay,
                            const Intersection intersection,
                            MonteCarloUnit& monteCarloUnit) const
{
  // Generate a ray in a random direction,
  // originating from the intersection
  Ray newRay;
  newRay.direction = monteCarloUnit.GetCosineDistributedHemisphereVector();

  // However, the new ray is now facing in the wrong direction,
  // it must be rotated towards the surface normal.
  newRay.direction = RotateTowards(newRay.direction,
    Dot(incomingRay.direction, intersection.normal) < 0.0f ?
    intersection.normal : -intersection.normal);

  // The probability of this ray is handled in the weighted distribution,
  // so no need for correction factors here.
  newRay.probability = 1.0f;

  newRay.wavelength = incomingRay.wavelength;
  newRay.origin = intersection.position;
  
  return newRay;
}

// --------------------

DiffuseGreyMaterial::DiffuseGreyMaterial(const float refl)
  : reflectance(refl) { }

Ray DiffuseGreyMaterial::GetNewRay(const Ray incomingRay,
                                   const Intersection intersection,
                                   MonteCarloUnit& monteCarloUnit) const
{
  Ray newRay = ClayMaterial::GetNewRay(incomingRay, intersection, monteCarloUnit);
  newRay.probability *= reflectance;
  return newRay;
}

// --------------------

DiffuseColouredMaterial::DiffuseColouredMaterial(const float refl,
                                                 const float wavel,
                                                 const float dev)
  : DiffuseGreyMaterial(refl)
  , wavelength(wavel)
  , deviation(dev) { }

Ray DiffuseColouredMaterial::GetNewRay(const Ray incomingRay,
                                       const Intersection intersection,
                                       MonteCarloUnit& monteCarloUnit) const
{
  float p = (wavelength - incomingRay.wavelength) / deviation;
  float q = std::exp(-0.5f * p * p);
  
  Ray newRay = DiffuseGreyMaterial::GetNewRay(incomingRay,
    intersection, monteCarloUnit);
  newRay.probability *= q;
  return newRay;
}

// --------------------

Ray PerfectMirrorMaterial::GetNewRay(const Ray incomingRay,
                                     const Intersection intersection,
                                     MonteCarloUnit&) const
{
  // Generate a ray in the reflected direction
  Ray newRay;
  newRay.direction = Reflect(incomingRay.direction, intersection.normal);

  // There is only one way in which the ray can be reflected,
  // so the probability of this happening is 1
  newRay.probability = 1.0f;

  newRay.wavelength = incomingRay.wavelength;
  newRay.origin = intersection.position;
  
  return newRay;
}

// --------------------

GlossyMirrorMaterial::GlossyMirrorMaterial(const float gloss)
  : glossiness(gloss) { }

Ray GlossyMirrorMaterial::GetNewRay(const Ray incomingRay,
                                    const Intersection intersection,
                                    MonteCarloUnit& monteCarloUnit) const
{
  Ray newRay;

  // Generate a ray in the reflected direction
  Vector3 reflection = Reflect(incomingRay.direction, intersection.normal);

  // And a diffuse ray to blend with
  Vector3 diffuse = monteCarloUnit.GetCosineDistributedHemisphereVector();

  // However, the diffuse ray is now facing in the wrong direction,
  // it must be rotated towards the surface normal.
  diffuse = RotateTowards(diffuse,
    Dot(incomingRay.direction, intersection.normal) < 0.0f ?
    intersection.normal : -intersection.normal);

  // Now the new ray direction is a mixture of the diffuse
  // and reflected ray
  newRay.direction = (diffuse * glossiness)
                   + (reflection * (1.0f - glossiness));
  newRay.direction.Normalise();

  // There is only one way in which the ray can be reflected, and the
  // diffuse reflection is weighted already, so the probability of this
  // happening is 1
  newRay.probability = 1.0f;

  newRay.wavelength = incomingRay.wavelength;
  newRay.origin = intersection.position;
  
  return newRay;
}

// --------------------

BrushedMetalMaterial::BrushedMetalMaterial(const float gloss,
                                           const float aniso)
  : glossiness(gloss)
  , anisotropy(aniso) { }

Ray BrushedMetalMaterial::GetNewRay(const Ray incomingRay,
                                    const Intersection intersection,
                                    MonteCarloUnit& monteCarloUnit) const
{
  Ray newRay;

  // Generate a ray in the reflected direction
  Vector3 reflection = Reflect(incomingRay.direction, intersection.normal);

  // And a diffuse ray to blend with
  Vector3 diffuse = monteCarloUnit.GetCosineDistributedHemisphereVector();

  // However, the diffuse ray is now facing in the wrong direction,
  // it must be rotated towards the surface normal.
  diffuse = RotateTowards(diffuse,
    Dot(incomingRay.direction, intersection.normal) < 0.0f ?
    intersection.normal : -intersection.normal);

  // A part of the binormal component must be removed, proportionally to
  // the anisotropy
  const float tangentialComponent = Dot(diffuse, intersection.tangent);
  diffuse = diffuse - (intersection.tangent * tangentialComponent) * anisotropy;
  diffuse.Normalise();

  // The new ray direction is a mixture of the diffuse and reflected ray
  newRay.direction = (diffuse * glossiness)
                   + (reflection * (1.0f - glossiness));
  newRay.direction.Normalise();

  // There is only one way in which the ray can be reflected, and the
  // diffuse reflection is weighted already, so the probability of this
  // happening is 1
  newRay.probability = 1.0f;

  newRay.wavelength = incomingRay.wavelength;
  newRay.origin = intersection.position;
  
  return newRay;
}

// --------------------

Ray RefractiveMaterial::GetNewRay(const Ray incomingRay,
                                  const Intersection intersection,
                                  MonteCarloUnit&) const
{
  // Generate a ray in the refracted direction
  Ray newRay;

  float cosI = -Dot(incomingRay.direction, intersection.normal);

  // Retrieve the index of refraction to be used
  // (which can be wavelength-dependent)
  float indexOfRefraction = GetIndexOfRefraction(incomingRay.wavelength);
  Vector3 normal = intersection.normal;

  // The IOR in this formula is n1 / n2, where n1 is air (1.0) when the
  // ray enters, otherwise, when the ray leaves the material, the IOR is
  // correct as is
  if (cosI > 0.0f)
  {
    indexOfRefraction = 1.0f / indexOfRefraction;
  }
  else
  {
    // The formula below assumes the normal to be at the same side as
    // the incident ray, if this is not the case, simply reverse the
    // normal
    normal = -intersection.normal;
    cosI = -cosI;
  }
  
  const float sinThetaSquared = indexOfRefraction * indexOfRefraction
                              * (1.0f - cosI * cosI);

  // When refraction is impossible,
  // total internal reflection must have occured
  if (sinThetaSquared > 1.0f)
  {
    newRay.direction = Reflect(incomingRay.direction, intersection.normal);
  }
  else
  {
    // Otherwise, compute the refracted ray
    const float cosT = std::sqrt(1.0f - sinThetaSquared);
    newRay.direction = indexOfRefraction * incomingRay.direction
                     + (indexOfRefraction * cosI - cosT) * normal;
  }

  // There is only one way in which the ray can be refracted,
  // so the probability of this happening is 1
  newRay.probability = 1.0f; 

  newRay.wavelength = incomingRay.wavelength;
  newRay.origin = intersection.position;
  
  return newRay;
}

// --------------------

float Bk7GlassMaterial::GetIndexOfRefraction(const float wavelength) const
{
  // See http://refractiveindex.info/?group=GLASSES&material=BK7

  // Square and convert nanometer to micrometer
  const double w2 = wavelength * wavelength * 1.0e-6;

  return (float)std::sqrt(
    1.0 + 1.03961212  * w2 / (w2 - 0.00600069867)
        + 0.231792344 * w2 / (w2 - 0.0200179144)
        + 1.01046945  * w2 / (w2 - 103.560653)
    );
}

// --------------------

float Sf10GlassMaterial::GetIndexOfRefraction(const float wavelength) const
{

  // See http://refractiveindex.info/?group=GLASSES&material=SF11

  // Square and convert nanometer to micrometer
  const double w2 = wavelength * wavelength * 1.0e-6;

  return (float)std::sqrt(
    1.0 + 1.73759695  * w2 / (w2 - 0.013188707)
        + 0.313747346 * w2 / (w2 - 0.0623068142)
        + 1.89878101  * w2 / (w2 - 155.23629)
    );
}

// --------------------

Ray SoapBubbleMaterial::GetNewRay(const Ray incomingRay,
                                  const Intersection intersection,
                                  MonteCarloUnit& monteCarloUnit) const
{
  Ray newRay;

  const float cosAlpha = Dot(incomingRay.direction, intersection.normal);

  // Reflect or pass through,
  // based on the angle between the ray and the normal
  if (monteCarloUnit.GetUnit() - 0.3f > std::abs(cosAlpha))
  {
    // When the angle between the normal and the ray is almost
    // 90 degrees, reflect
    newRay.direction = Reflect(incomingRay.direction, intersection.normal);
  }
  else
  {
    // Otherwise, simply pass it through, because the bubble is filled
    // with air, so it does not refract anything
    newRay.direction = incomingRay.direction;
  }

  // Take a phase shift from 0 - 2pi based on the wavelength
  const float phaseShift = (incomingRay.wavelength - 380.0f)
                         / 200.0f * (float)pi;

  // Then calculate the probability for this wavelength based on the
  // angles between rays and the normal. Please note that this is by no
  // means accurate; it simply looks cool :)
  const float cosPhi = std::min(0.999f, std::max(-0.999f,
    Dot(newRay.direction, intersection.normal)));
  const float cosTheta = std::min(0.999f, std::max(-0.999f,
    Dot(newRay.direction, intersection.tangent)));
  newRay.probability = std::cos(phaseShift - std::acos(cosPhi) * 3.0f
                                - std::acos(cosTheta) * 2.0f
                                + (float)pi * 0.5f) * 0.1f + 0.9f;

  newRay.wavelength = incomingRay.wavelength;
  newRay.origin = intersection.position;
  
  return newRay;
}

// --------------------

Ray IridescentMaterial::GetNewRay(const Ray incomingRay,
                                  const Intersection intersection,
                                  MonteCarloUnit& monteCarloUnit) const
{
  Ray newRay;

   // Generate a ray in the reflected direction
  Vector3 reflection = Reflect(incomingRay.direction, intersection.normal);

  // And a diffuse ray to blend with
  Vector3 diffuse = monteCarloUnit.GetCosineDistributedHemisphereVector();

  // However, the diffuse ray is now facing in the wrong direction,
  // it must be rotated towards the surface normal.
  diffuse = RotateTowards(diffuse,
    Dot(incomingRay.direction, intersection.normal) < 0.0f ?
    intersection.normal : -intersection.normal);

  // Now the new ray direction is a random mixture of the diffuse and
  // reflected ray
  const float glossiness = monteCarloUnit.GetUnit();
  newRay.direction = (diffuse * glossiness)
                   + (reflection * (1.0f - glossiness));
  newRay.direction.Normalise();

  // Take a phase shift from 0 - 2pi based on the wavelength
  const float phaseShift = (incomingRay.wavelength - 380.0f)
                         / 200.0f * (float)pi;

  // Then calculate the probability for this wavelength based on the
  // angles between rays and the normal. Please note that this is by n
  // means accurate; it simply looks cool :)
  const float cosPhi   = std::min(0.999f, std::max(-0.999f,
    Dot(newRay.direction, intersection.normal)));
  const float cosTheta = std::min(0.999f, std::max(-0.999f,
    Dot(newRay.direction, intersection.tangent)));
  newRay.probability = std::cos(phaseShift + std::acos(cosPhi) * 3.0f
                            - std::acos(cosTheta) * 2.0f) * 0.5f + 0.5f;

  newRay.wavelength = incomingRay.wavelength;
  newRay.origin = intersection.position;
  
  return newRay;
}
