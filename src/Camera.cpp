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

#include "Camera.h"

#include <cmath>
#include "Constants.h"
#include "Quaternion.h"
#include "MonteCarloUnit.h"

using namespace Luculentus;

Ray Camera::GetScreenRay(const float x, const float y,
                         const float chromaticAberrationFactor,
                         const float dofAngle,
                         const float dofRadius) const
{
  // The smaller the FOV, the further the screen is away;
  // the larger the FOV, the closer the screen is.
  float screenDistance = 1.0f / std::tan(fieldOfView * 0.5f);
  Vector3 direction = { x, screenDistance, -y };

  // Then apply some wavelength dependent zoom to create chromatic
  // aberration. Please note, this is not a physically correct model of
  // chromatic aberration, for a correct response, you can place a lens
  // in front of the camera, with a dispersive glass material.
  direction.x *= chromaticAberrationFactor;
  direction.z *= chromaticAberrationFactor;
  
  // Now find the intersection with the focal plane (which is trivial as
  // long as the ray is not transformed yet).
  direction.Normalise();
  Vector3 focusPoint = direction * (focalDistance / direction.y);

  // Then take a new point on the camera 'lens' (this is of course not
  // accurate, but then again, the pinhole camera does not have depth of
  // field at all, so it is a hack anyway).
  Vector3 lensPoint =
  {
    std::cos(dofAngle) * dofRadius,
    0.0f,
    std::sin(dofAngle) * dofRadius
  };
  
  // Then construct the new ray,
  // from the lens point through the focus point.
  direction = focusPoint - lensPoint;
  Ray r;
  r.direction = Rotate(direction, orientation);
  r.origin = position + Rotate(lensPoint, orientation);
  r.direction.Normalise();

  return r;
}

Ray Camera::GetRay(const float x, const float y, const float wavelength,
                   MonteCarloUnit& monteCarloUnit) const
{
  // Pick depth of field coordinates randomly.
  const float dofAngle = monteCarloUnit.GetLongitude();
  const float dofRadius = monteCarloUnit.GetUnit() / depthOfField;

  // Calculate a zoom factor based on the wavelenth
  // to simulate chromatic aberration of the lens.
  const float d = (wavelength - 580.0f) / 200.0f;
  const float chromaticZoom = 1.0f + d * chromaticAberration;

  Ray r = GetScreenRay(x, y, chromaticZoom, dofAngle, dofRadius);
  r.wavelength = wavelength;

  r.probability = 1.0f;

  return r;
}
