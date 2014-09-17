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

#include "Vector3.h"
#include "Quaternion.h"
#include "Ray.h"

namespace Luculentus
{
  class MonteCarloUnit;

  class Camera
  {
    public:

      /// Location of the camera in the scene.
      Vector3 position;

      /// Horizontal field of view in radians.
      float fieldOfView;

      /// The distance along the optical axis
      /// that is perfectly in focus.
      float focalDistance;

      /// The amount of depth of field. A large value indicates that all
      /// objects are sharp. A shallow depth of field (a small value),
      /// means lots of blurring for out-of-focus objects.
      float depthOfField;

      /// The amount of chromatic aberration. 0 indicates no chromatic
      /// aberration, larger values result in more chromatic aberration.
      float chromaticAberration;

      /// The direction in which the camera is looking.
      Quaternion orientation;

      /// Returns a camera ray through the screen at the specified
      /// position, where -1.0 is left and 1.0 right, with square units.
      Ray GetRay(const float x, const float y, const float wavelength,
                 MonteCarloUnit& monteCarloUnit) const;

    private:

      /// Returns a ray through the screen,
      /// where -1.0 is left, 1.0 is right, and the units are square.
      Ray GetScreenRay(const float x, const float y,
                       const float chromaticAberrationFactor,
                       const float dofAngle,
                       const float dofRadius) const;
  };
}
