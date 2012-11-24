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

#include "Compound.h"

#include "Constants.h"

using namespace Luculentus;

ConvexLens Luculentus::MakeConvexLens(const Vector3 position,
                                      const Vector3 axis,
                                      const float thickness,
                                      const float focalLength,
                                      const float indexOfRefraction)
{
  // Solve the lensmaker's equation for the sphere radius
  const float d = focalLength * (indexOfRefraction - 1.0f)
                * (indexOfRefraction - 1.0f) * indexOfRefraction
                * (focalLength * indexOfRefraction - thickness);

  const float r1 = (std::sqrt(d)
                 + focalLength * indexOfRefraction * indexOfRefraction
                 - focalLength * indexOfRefraction) / indexOfRefraction;

  const float r2 = -(std::sqrt(d)
                 + focalLength * indexOfRefraction * indexOfRefraction
                 - focalLength * indexOfRefraction) / indexOfRefraction;

  // Create the two spheres that will form the lens
  Sphere s1(position + axis * (r1 - thickness * 0.5f), r1);
  Sphere s2(position - axis * (r1 - thickness * 0.5f), r2);

  return ConvexLens(s1, s2);
}

InfinitePrism Luculentus::MakeInfinitePrism(const Vector3 axis,
                                            const Vector3 offset,
                                            const float edgeLength,
                                            const float angle )
{
  const float radius = std::sqrt(3.0f) / 6.0f * edgeLength;
  const float a1 = angle;
  const float a2 = angle + (float)pi * 2.0f / 3.0f;
  const float a3 = angle + (float)pi * 4.0f / 3.0f;

  // Calculate three unit vertices on the xy-plane
  Vector3 p1 = { std::cos(a1), std::sin(a1), 0.0f };
  Vector3 p2 = { std::cos(a2), std::sin(a2), 0.0f };
  Vector3 p3 = { std::cos(a3), std::sin(a3), 0.0f };

  // Now rotate the vertices,
  // so they lie in the plane of which the axis vector is the normal
  p1 = RotateTowards(p1, axis);
  p2 = RotateTowards(p2, axis);
  p3 = RotateTowards(p3, axis);

  // Then the planes through the vectices can be constructed
  SpacePartitioning sp1 = SpacePartitioning(p1, p1 * radius + offset);
  SpacePartitioning sp2 = SpacePartitioning(p2, p2 * radius + offset);
  SpacePartitioning sp3 = SpacePartitioning(p3, p3 * radius + offset);

  // And finally combine the space partitionings
  // so that the extruded triangle is 'carved out'
  return InfinitePrism(
           IntersectionCompound<SpacePartitioning, SpacePartitioning>
             (sp1, sp2), sp3
         );
}

ThickPlane Luculentus::MakeThickPlane( const Vector3 normal,
                                       const Vector3 offset,
                                       const float thickness )
{
  return ThickPlane(
           // Plane is extruded along the normal,
           // so the plane through the offset vector
           // should have the opposite normal
           SpacePartitioning(-normal, offset),
           // And the extruded plane has the normal
           // in the direction of the extrusion
           SpacePartitioning(normal, offset + normal * thickness)
         );
}

Prism Luculentus::MakePrism(const Vector3 axis, const Vector3 offset,
                            const float edgeLength, const float angle,
                            const float thickness)
{
  return Prism(
           // The infinite prism as before
           MakeInfinitePrism(axis, offset, edgeLength, angle),
           // But now clipped by a thick plane along the axis
           MakeThickPlane(axis, offset, thickness)
         );
}

HexagonalPrism Luculentus::MakeHexagonalPrism(const Vector3 axis,
                                              const Vector3 offset,
                                              const float edgeLength,
                                              const float bevelSize,
                                              const float angle,
                                              const float thickness)
{
  return HexagonalPrism(
           // The 'bevel edges' (which is just an infinitely extruded
           // triangle; an infinte prism). It is rotated 180 degrees,
           // so it cuts off the corners. If the edge length is twice
           // the edge length of the desired prism, it does not cut the
           // corners at all.
           MakeInfinitePrism(axis, offset,
                             edgeLength * 2.0f - bevelSize * 3.0f,
                             angle + static_cast<float>(pi)),
           // And the normal prism, without bevel
           MakePrism(axis, offset, edgeLength, angle, thickness)
         );
}
