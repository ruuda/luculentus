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

#include "Surface.h"

#include "MonteCarloUnit.h"

using namespace Luculentus;

Plane::Plane(const Vector3 n, const Vector3 o)
  : normal(n)
  , offset(o) { }

Plane::Plane(const Plane& other)
  : normal(other.normal)
  , offset(other.offset) { }

bool Plane::Intersect(const Ray ray, Intersection& intersection) const
{
  // Transform the ray into the space where the plane is a linear
  // subspace (a plane through the origin)
  Vector3 localOrigin = ray.origin - offset;

  float t = - Dot(normal, localOrigin) / Dot(normal, ray.direction);

  // A ray has one direction only, do not hit backwards
  if (t <= 0.0f) return false;

  // Fill in the intersection details
  intersection.distance = t;
  float sign = Dot(normal, ray.direction);
  // Planes are two-sided
  intersection.normal = sign < 0.0f ? normal : -normal;
  intersection.position = ray.origin + t * ray.direction;

  return true;
}

// --------------------

SpacePartitioning::SpacePartitioning(const Vector3 n, const Vector3 o)
  : Plane(n, o) { }

SpacePartitioning::SpacePartitioning(const SpacePartitioning& other)
  : Plane(other) { }

bool SpacePartitioning::Intersect(const Ray ray,
                                  Intersection& intersection) const
{
  // Transform the ray into the space where the plane is a linear
  // subspace (a plane through the origin)
  Vector3 localOrigin = ray.origin - offset;

  float t = - Dot(normal, localOrigin) / Dot(normal, ray.direction);

  // A ray has one direction only, do not hit backwards
  if (t <= 0.0f) return false;

  // Fill in the intersection details
  intersection.distance = t;
  intersection.normal = normal; // A space partitioning is one-sided
  intersection.position = ray.origin + t * ray.direction;

  return true;
}

bool SpacePartitioning::LiesInside(const Vector3 x) const
{
  return Dot(x - offset, normal) < 0.0f;
}

// --------------------

Circle::Circle(const Vector3 n, const Vector3 o, const float r)
  : Plane(n, o)
  , radiusSquared(r * r)
  , radius(r) { }

Circle::Circle(const Circle& other)
  : Plane(other)
  , radiusSquared(other.radiusSquared)
  , radius(other.radius) { }

bool Circle::Intersect(const Ray ray, Intersection& intersection) const
{
  // If the ray intersects the plane in which the circle lies
  if (Plane::Intersect(ray, intersection))
  {
    // Then the intersection must lie within the circle
    return (intersection.position - offset)
      .MagnitudeSquared() <= radiusSquared;
  }

  return false;
}

// --------------------

Sphere::Sphere(const Vector3 p, const float r)
  : position(p)
  , radiusSquared(r * r) { }

Sphere::Sphere(const Sphere& other)
  : position(other.position)
  , radiusSquared(other.radiusSquared) { }

bool Sphere::Intersect(const Ray ray, Intersection& intersection) const
{
  float t1, t2;
  
  // Find intersection points; if there are none, return no intersection
  if (!GetIntersections(position, radiusSquared,
        ray.origin, ray.direction, t1, t2)) return false;

  float t = 0.0f;

  // One of the ts must be positive at least
  if (t1 > 0.0f && t1 < t2) t = t1;
  else if (t2 > 0.0f && t2 < t1) t = t2;
  // For negative t, the spehere lies behind the ray entirely
  else return false;

  // Distance is equal to t, and the intersection can be calculated
  // from here
  intersection.position = ray.direction * t + ray.origin;
  intersection.distance = t;

  // The normal points radially outward everywhere
  intersection.normal = intersection.position - position;
  intersection.normal.Normalise();

  // The tangent vector is simply perpendicular to the surface normal
  // and the up vector. This choice is quite arbitrary.
  Vector3 up = { 0.0f, 1.0f, 0.0f };
  intersection.tangent = Cross(up, intersection.normal);
  intersection.tangent.Normalise();

  // The intersection has been found now
  return true;
}

bool Sphere::LiesInside(const Vector3 x) const
{
  return (x - position).MagnitudeSquared() < radiusSquared;
}

bool Sphere::GetIntersections(const Vector3 spherePosition,
                              const float sphereRadiusSquared,
                              const Vector3 rayOrigin,
                              const Vector3 rayDirection,
                              float& t1, float& t2)
{
  // Compute a, b, c factors of the quadratic equation
  const float a = 1.0f;
  const Vector3 centreDifference = spherePosition - rayOrigin;
  const float b = 2.0f * Dot(rayDirection, centreDifference);
  const float c = centreDifference.MagnitudeSquared()
                - sphereRadiusSquared;

  // The discriminant determines whether the equation has a solution
  const float discriminant = b * b - 4.0f * a * c;

  // If it less than zero, there is no intersection
  if (discriminant < 0.0f) return false;

  // Otherwise, the equation can be solved for t.
  const float sqrtD = std::sqrt(discriminant);
  t1 = -0.5f * (-b + sqrtD) / a;
  t2 = -0.5f * (-b - sqrtD) / a;

  return true;
}

// --------------------

Paraboloid::Paraboloid(const Vector3 n, const Vector3 o,
                       const float focalDistance)
  : offset(o - n * focalDistance)
  , normal(n)
  , focalPoint(n * (focalDistance * 2.0f)) { }

Paraboloid::Paraboloid(const Paraboloid& other)
  : offset(other.offset)
  , normal(other.normal)
  , focalPoint(other.focalPoint) { }

bool Paraboloid::Intersect(const Ray ray, Intersection& intersection) const
{
  float t;

  // Transform the ray into the space where the plane is a linear
  // subspace (a plane through the origin)
  const Vector3 localOrigin = ray.origin - offset;

  const float nDotD = Dot(normal, ray.direction);
  const float nDotO = Dot(normal, localOrigin);
  const Vector3 focalOffset = localOrigin - focalPoint;

  // Calculate a, b and c for the quadratic paraboloid equation
  const float a = nDotD * nDotD - 1.0f;
  const float b = 2.0f * nDotD * nDotO - 2.0f * Dot(ray.direction, focalOffset);
  const float c = nDotO * nDotO - focalOffset.MagnitudeSquared();

  // If a is zero, there is no quadratic equation, and the solution is a
  // simple line intersection
  if (a == 0.0f)
  {
    t = -c / b;
    // For negative t, the paraboloid lies behind the ray
    if (t < 0.0f) return false;
  }
  else
  {
    // The discriminant determines whether the equation has a solution
    const float discriminant = b * b - 4.0f * a * c;

    // If it less than zero, there is no intersection
    if (discriminant < 0.0f) return false;

    // Otherwise, the equation can be solved for t.
    const float sqrtD = std::sqrt(discriminant);
    const float t1 = 0.5f * (-b + sqrtD) / a;
    const float t2 = 0.5f * (-b - sqrtD) / a;

    // Pick the closest non-negative t
    if (t1 > 0.0f && (t1 < t2 || t2 <= 0.0f)) t = t1;
    else if (t2 > 0.0f) t = t2;
    // For negative t, the paraboloid lies behind the ray entirely
    else return false;
  }

  // Fill in the intersection details
  intersection.distance = t;
  intersection.position = ray.origin + t * ray.direction;

  // Now the normal can be computed
  const Vector3 localIntersection = intersection.position - offset;
  const Vector3 planeProjection = localIntersection - normal
                                * Dot(localIntersection, normal);
  intersection.normal = focalPoint - planeProjection;
  intersection.normal.Normalise();

  return true;
}

// --------------------

CappedParaboloid::CappedParaboloid(const Vector3 n, const Vector3 o,
                                   const float focalDistance,
                                   const float radius)
  : Paraboloid(n, o, focalDistance)
  , radiusSquared(radius * radius) { }

CappedParaboloid::CappedParaboloid(const CappedParaboloid& other)
  : Paraboloid(other)
  , radiusSquared(other.radiusSquared) { }

// TODO: DRY / can code be shared with the paraboloid?
bool CappedParaboloid::Intersect(const Ray ray,
                                 Intersection& intersection) const
{
  // Transform the ray into the space where the plane is a linear
  // subspace (a plane through the origin)
  const Vector3 localOrigin = ray.origin - offset;

  const float nDotD = Dot(normal, ray.direction);
  const float nDotO = Dot(normal, localOrigin);
  const Vector3 focalOffset = localOrigin - focalPoint;

  // Calculate a, b and c for the quadratic paraboloid equation
  const float a = nDotD * nDotD - 1.0f;
  const float b = 2.0f * nDotD * nDotO
                - 2.0f * Dot(ray.direction, focalOffset);
  const float c = nDotO * nDotO - focalOffset.MagnitudeSquared();

  // The discriminant determines whether the equation has a solution
  const float discriminant = b * b - 4.0f * a * c;

  // If it less than zero, there is no intersection
  if (discriminant < 0.0f) return false;

  // Otherwise, the equation can be solved for t.
  const float sqrtD = std::sqrt(discriminant);
  const float t1 = 0.5f * (-b + sqrtD) / a;
  const float t2 = 0.5f * (-b - sqrtD) / a;
  const Vector3 t1Intersection = localOrigin + t1 * ray.direction;
  const Vector3 t2Intersection = localOrigin + t2 * ray.direction;
  const Vector3 t1PlaneProjection = t1Intersection - normal
                                  * Dot(t1Intersection, normal);
  const Vector3 t2PlaneProjection = t2Intersection - normal
                                  * Dot(t2Intersection, normal);

  float t;
  Vector3 localIntersection;
  Vector3 planeProjection;
  // Pick the closest non-negative t, within the valid radius
  if (t1 > 0.0f && (t1 < t2 || t2 <= 0.0f)
      && t1PlaneProjection.MagnitudeSquared() < radiusSquared)
  {
    t = t1; localIntersection = t1Intersection;
    planeProjection = t1PlaneProjection;
  }
  else if (t2 > 0.0f
           && t2PlaneProjection.MagnitudeSquared() < radiusSquared)
  {
    t = t2; localIntersection = t2Intersection;
    planeProjection = t2PlaneProjection;
  }
  // For negative t, the paraboloid lies behind the ray entirely
  else return false;

  // Fill in the intersection details
  intersection.distance = t;
  intersection.position = localIntersection + offset;

  // Now the normal can be computed
  intersection.normal = focalPoint - planeProjection;
  intersection.normal.Normalise();

  // The paraboloid is two-sided.
  if (Dot(localOrigin, intersection.normal) < 0.0f)
  {
    intersection.normal = -intersection.normal;
  }

  return true;
}
