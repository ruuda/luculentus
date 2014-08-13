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

#include "Scene.h"

using namespace Luculentus;

const Object* Scene::Intersect(Ray ray, Intersection& intersection) const
{
  // Assume Nothing is found, and that Nothing is Very Far Away
  const Object* object = nullptr;
  intersection.distance = 1.0e12f;

  // Then intersect all surfaces
  for (auto& obj : objects)
  {
    Intersection currentIntersection;
    if (obj.surface->Intersect(ray, currentIntersection))
    {
      // If there is an intersection, and if it is nearer than a
      // previous one, use it.
      if (currentIntersection.distance < intersection.distance)
      {
        intersection = currentIntersection;
        object = &obj;
      }
    }
  }

  return object;
}
