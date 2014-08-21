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

#include "Surface.h"
#include "Volume.h"

namespace Luculentus
{
  template <typename T1, typename T2>
  class IntersectionCompound : public Surface, public Volume
  {
    public:

      /// The first of the two surfaces
      T1 surface1;

      /// The second of the two surfaces
      T2 surface2;

      /// Creates a new object,
      /// which is the intersection of the two specified objects
      IntersectionCompound(const T1& s1, const T2& s2)
        : surface1(s1)
        , surface2(s2)
      {

      }

      IntersectionCompound(const IntersectionCompound<T1, T2>& other)
        : surface1(other.surface1)
        , surface2(other.surface2)
      {

      }

      virtual bool Intersect(const Ray ray,
                             Intersection& intersection) const
      {
        // Intersect both surfaces
        Intersection i1, i2;

        bool surface1Intersected = surface1.Intersect(ray, i1);
        bool surface2Intersected = surface2.Intersect(ray, i2);

        // If not intersecting any of the two,
        // it is not intersecting the compound object either
        if (!surface1Intersected && !surface2Intersected) return false;

        // Invalidate intersections that do not lie inside
        // the intersection of the two volumes.
        if (surface1Intersected && !surface2.LiesInside(i1.position))
          surface1Intersected = false;
        if (surface2Intersected && !surface1.LiesInside(i2.position))
          surface2Intersected = false;

        // Again, if both intersections are now invalid,
        // there is no intersection
        if (!surface1Intersected && !surface2Intersected) return false;

        // If both intersections are valid, pick the closest one
        if (surface1Intersected && surface2Intersected)
        {
          if ((i1.position - ray.origin).MagnitudeSquared()
              < (i2.position - ray.origin).MagnitudeSquared())
            surface2Intersected = false;
          else surface1Intersected = false;
        }

        // Now pick the best intersection
        if (surface1Intersected) intersection = i1;
        else intersection = i2;
        return true;
      }

      virtual bool LiesInside(const Vector3 x) const
      {
        // The point must lie in both volumes to lie in its intersection
        return surface1.LiesInside(x) && surface2.LiesInside(x);
      }
  };

  typedef IntersectionCompound<Sphere, Sphere>
          ConvexLens;

  typedef IntersectionCompound<IntersectionCompound
          <SpacePartitioning, SpacePartitioning>, SpacePartitioning>
          InfinitePrism;

  typedef IntersectionCompound<SpacePartitioning, SpacePartitioning>
          ThickPlane;

  typedef IntersectionCompound<InfinitePrism, ThickPlane>
          Prism;

  typedef IntersectionCompound<InfinitePrism, Prism>
          HexagonalPrism;

  /// Constructs a new simple convex lens, with specified position and
  /// optical axis, a thickness through the centre, and the specified
  /// focal length. These properties depend on the index of refraction
  /// of the lens material used.
  ConvexLens MakeConvexLens(const Vector3 position, const Vector3 axis,
                            const float thickness,
                            const float focalLength,
                            const float indexOfRefraction);

  /// Constructs an equilateral triangle with specified edge length,
  /// infinitely extruded along the axis vector,
  /// rotated at the specified angle.
  InfinitePrism MakeInfinitePrism(const Vector3 axis,
                                  const Vector3 offset,
                                  const float edgeLength,
                                  const float angle);

  /// Constructs a thick, infinite 'wall'-like structure. One surface
  /// passes through the specified offset, the other one is translated
  /// along the normal for the specified thickness distance.
  ThickPlane MakeThickPlane(const Vector3 normal, const Vector3 offset,
                            const float thickness);

  /// Constructs a prism, oriented along the specified axis, its
  /// equilateral base triangle with specified edge length at the
  /// specified offset, rotated with the specified angle, and then
  /// extruded along the axis for the specified height.
  Prism MakePrism(const Vector3 axis, const Vector3 offset,
    const float edgeLength, const float angle, const float height);

  /// Constructs a prism, oriented along the specified axis, its
  /// equilateral base triangle with specified edge length at the
  /// specified offset, rotated with the specified angle, and then
  /// extruded along the axis for the specified height. Then its corners
  /// are capped, such that the effective edge length becomes the edge
  /// length minus twice the bevel size.
  HexagonalPrism MakeHexagonalPrism(const Vector3 axis,
    const Vector3 offset, const float edgeLength, const float bevelSize,
    const float angle, const float height);
}
