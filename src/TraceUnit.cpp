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

#include "TraceUnit.h"

#include "Scene.h"

using namespace Luculentus;

TraceUnit::TraceUnit(const Scene* const scn,
                     const unsigned long randomSeed, const int width,
                     const int height)
  : monteCarloUnit(randomSeed)
  , scene(scn)
  , aspectRatio(static_cast<float>(width) / static_cast<float>(height))
{

}

void TraceUnit::Render()
{
  for (auto& mappedPhoton : mappedPhotons)
  {
    // Pick a wavelength for this photon
    const float wavelength = monteCarloUnit.GetWavelength();

    // Pick a screen coordinate for the photon
    const float x = monteCarloUnit.GetBiUnit();
    const float y = monteCarloUnit.GetBiUnit() / aspectRatio;

    // Store the pixel coordinates already
    mappedPhoton.wavelength = wavelength;
    mappedPhoton.x = x;
    mappedPhoton.y = y;
    
    // And then trace the scene at this wavelength
    mappedPhoton.probability = RenderCameraRay(x, y, wavelength);
  }
}

float TraceUnit::RenderCameraRay(const float x, const float y,
                                 const float wavelength)
{
  // Get a random time to sample at
  const float t = monteCarloUnit.GetUnit();

  // Get the camera at that time
  const Camera camera = scene->GetCameraAtTime(t);

  // Create a camera ray for the specified pixel and wavelength
  Ray ray = camera.GetRay(x, y, wavelength, monteCarloUnit);
  
  // And render this new camera ray
  return RenderRay(ray);
}

float TraceUnit::RenderRay(Ray ray)
{
  // The path starts with the ray,
  // and there is a chance it continues
  float continueChance = 1.0f;

  // Apart from the chance, which might decrease even for specular
  // bounces, light intensity is affected only by interaction
  // probabilities
  float intensity = 1.0f;

  do
  {
    // Intersect the ray with the scene
    Intersection intersection;
    const Object* object = scene->Intersect(ray, intersection);

    // If nothing was intersected, the path ends,
    // and the only thing left is the utter darkness of The Void
    if (!object) return 0.0f;

    // If a light was hit, the path ends,
    // and the intensity of the light determines the intensity of the path.
    if (!object->material)
    {
      return intensity * object->emissiveMaterial->GetIntensity(ray.wavelength);
    }

    // Otherwise, the ray must have hit a non-emissive surface,
    // and so the journey continues ...
    ray = object->material->GetNewRay(ray, intersection, monteCarloUnit);
    intensity *= ray.probability;

    // Displace the origin slightly, so the new ray won't intersect the
    // same point
    ray.origin = ray.origin + ray.direction * 0.00001f;

    // And the chance of a new bounce decreases slightly
    continueChance *= 0.96f;
  }
  // Use a sharp falloff based on intensity, so an intensity of
  // 0.1 still has 86% chance of continuing, but an intensity of
  // 0.01 has only 18% chance of continuing
  while (monteCarloUnit.GetUnit() * 0.85f < continueChance
         * (1.0f - std::exp(intensity * -20.0f)));

  // If Russian roulette terminated the path,
  // there is always an option of trying direct illumination,
  // which could be implemented here,
  // but is not

  return 0.0f;
}
