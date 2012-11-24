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
#include "Quaternion.h"
#include "Intersection.h"
#include "Volume.h"

namespace Luculentus
{
  class Surface
  {
    public:

      /// Returns whether the surface was intersected, and if so, where.
      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const = 0;
  };

  class Plane : public Surface
  {
    public:

      /// A vector of length 1 perpendicular to the plane
      const Vector3 normal;

      /// A point in the plane
      const Vector3 offset;

      /// Creates a new plane with the specified normal and offset
      Plane(const Vector3 n, const Vector3 o);

      /// Copy constructor
      Plane(const Plane& other);

      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const;
  };

  /// Like a one-sided plane, something that cuts space in half.
  class SpacePartitioning : public Plane, public Volume
  {
    public:

      /// Creates a new plane with the specified normal and offset
      SpacePartitioning(const Vector3 n, const Vector3 o);

      /// Copy constructor
      SpacePartitioning(const SpacePartitioning& other);

      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const;

      virtual bool LiesInside(const Vector3 x) const;
  };

  class Circle : public Plane
  {
    public:

      /// The circle radius squared
      const float radiusSquared;

      /// The circle radius
      const float radius;

      /// Creates a new circle along the plane defined by the normal and
      /// offset, with the specified radius
      Circle(const Vector3 n, const Vector3 o, const float r);

      /// Copy constructor
      Circle(const Circle& other);

      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const;
  };

  class Sphere : public Surface, public Volume
  {
    public:

      /// The position of the centre of the sphere
      const Vector3 position;
      
      /// The sphere radius squared
      const float radiusSquared;

      Sphere(const Vector3 pos, const float r);

      /// Copy constructor
      Sphere(const Sphere& other);

      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const;

      virtual bool LiesInside(const Vector3 x) const;

      /// Returns whether a ray intersects a sphere, and if it does,
      /// it returns the distances along the ray in t1 and t2
      static bool GetIntersections(const Vector3 spherePosition,
                                   const float sphereRadiusSquared,
                                   const Vector3 rayOrigin,
                                   const Vector3 rayDirection,
                                   float& t1, float& t2);
  };

  class Paraboloid : public Surface
  {
    public:

      /// The position of the focal point projected onto the plane
      const Vector3 offset;

      /// The normal of the plane, a vector of length 1 perpendicular to
      /// the plane (pointed towards the focal point)
      const Vector3 normal;

      /// The position of the focal point, relative to the offset point.
      const Vector3 focalPoint;

      /// Creates a new paraboloid, for the plane with the specified
      /// normal, the top centered at the specified offset, and
      /// specified focal distance (from the top, not the plane).
      Paraboloid(const Vector3 n, const Vector3 o,
                 const float focalDistance);

      /// Copy constructor
      Paraboloid(const Paraboloid& other);

      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const;
  };

  class CappedParaboloid : public Paraboloid
  {
    public:

      /// The maximum radius of the circle obtained by intersecting a
      /// plane perpendicular to the paraboloid plane with the paraboloid.
      const float radiusSquared;

      /// Creates a new paraboloid, for the plane with the specified
      /// normal, top centered at the specified offset, and specified
      /// focal distance (from the top, not the plane), with a limited
      /// radius.
      CappedParaboloid(const Vector3 n, const Vector3 o,
                       const float focalDistance, const float radius);

      /// Copy constructor
      CappedParaboloid(const CappedParaboloid& other);

      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const;
  };
}
