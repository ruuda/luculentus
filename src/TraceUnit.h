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

#include "MappedPhoton.h"
#include "Ray.h"
#include "Object.h"
#include "Intersection.h"
#include "MonteCarloUnit.h"

namespace Luculentus
{
  class Scene;

  class TraceUnit
  {
    public:

      /// The random number generator
      MonteCarloUnit monteCarloUnit;

      /// The scene that will be rendered.
      const Scene* const scene;

      /// The aspect ratio of the image that will be rendered
      const float aspectRatio;

      // Trace less paths per task in debug mode, because debug mode is
      // terribly slow
      #ifdef _DEBUG
      static const int numberOfPaths = 1024 * 64;
      #else
      static const int numberOfPaths = 1024 * 512;
      #endif

      static const int numberOfMappedPhotons = numberOfPaths;

      /// The photons that were rendered
      MappedPhoton mappedPhotons[numberOfMappedPhotons];

      /// Creates a new work unit that renders the specified scene,
      /// initialized with the specified random seed
      TraceUnit(const Scene* const scn, const unsigned long randomSeed,
                const int width, const int height);

      /// Fills the buffer of mapped photons once.
      void Render();

    private:

      /// Returns the contribution of a ray through the specified screen
      /// coordinates.
      float RenderCameraRay(const float x, const float y,
                            const float wavelength);

      /// Retruns the contribution of a photon travelling backwards the
      /// specified ray.
      float RenderRay(Ray ray);
  };
}
