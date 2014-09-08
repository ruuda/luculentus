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

#include <vector>
#include <functional>
#include "Camera.h"
#include "Ray.h"
#include "Object.h"

namespace Luculentus
{
  class Scene
  {
    public:

      /// All the renderable objects in the scene.
      std::vector<Object> objects;

      /// A function that returns the camera through which the scene
      /// will be seen. The function takes one parameter, the time (in
      /// the range 0.0 – 1.0), which will be sampled randomly to create
      /// effects like motion blur and zoom blur.
      std::function<Camera (const float)> GetCameraAtTime;

      /// Intersects the specified ray with the scene. If an object is
      /// intersected, it is returned, and the intersection is set.
      const Object* Intersect(Ray ray, Intersection& intersection) const;
  };
}
