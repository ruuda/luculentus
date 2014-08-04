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
#include <cmath>

namespace Luculentus
{
  struct Vector3;

  float Dot(const Vector3 a, const Vector3 b);

  Vector3 Cross(const Vector3 a, const Vector3 b);

  struct Vector3
  {
    float x, y, z;

    inline float MagnitudeSquared() const
    {
      return Dot(*this, *this);
    }

    inline float Magnitude() const
    {
      return std::sqrt(MagnitudeSquared());
    }

    inline void Normalise()
    {
      float magnitude = Magnitude();
        
      // Ignore the 0-vector
      if (magnitude == 0.0f) return;

      x /= magnitude;
      y /= magnitude;
      z /= magnitude;
    }
  };

  inline Vector3 operator+(const Vector3 a, const Vector3 b)
  {
    Vector3 sum = { a.x + b.x, a.y + b.y, a.z + b.z };
    return sum;
  }

  inline Vector3 operator-(const Vector3 a, const Vector3 b)
  {
    Vector3 diff = { a.x - b.x, a.y - b.y, a.z - b.z };
    return diff;
  }

  inline Vector3 operator-(const Vector3 a)
  {
    Vector3 neg = { -a.x, -a.y, -a.z };
    return neg;
  }

  inline Vector3 operator*(const Vector3 a, const float f)
  {
    Vector3 prod = { a.x * f, a.y * f, a.z * f };
    return prod;
  }

  inline Vector3 operator*(const float f, const Vector3 a)
  {
    return a * f;
  }

  inline float Dot(const Vector3 a, const Vector3 b)
  {
    return  a.x * b.x + a.y * b.y + a.z * b.z;
  }

  inline Vector3 Cross(const Vector3 a, const Vector3 b)
  {
    Vector3 cross = { a.y * b.z - a.z * b.y,
                      a.z * b.x - a.x * b.z,
                      a.x * b.y - a.y * b.x };
    return cross;
  }

  // Transforms a vector such that the positive z-axis points along the
  // normal
  inline Vector3 RotateTowards(Vector3 a, const Vector3 normal)
  {
    // Based on http://stackoverflow.com/questions/1171849/finding-quaternion-representing-the-rotation-from-one-vector-to-another

    float dot = normal.z;
    if (dot >  0.9999f) return a; // No rotation necessary.
    if (dot < -0.9999f) { a.z *= -1.0f; return a; } // Mirror along the z-axis.

    Vector3 up = { 0.0f, 0.0f, 1.0f };
    Vector3 a1 = Cross(up, normal); a1.Normalise();
    Vector3 a2 = Cross(a1, normal); a2.Normalise();
    Vector3 r = a.x * a1 + a.y * a2 + a.z * normal;

    return r;
  }

  /// Reflects a vector in the plane through the origin with the specified normal
  inline Vector3 Reflect(const Vector3 a, const Vector3 normal)
  {
    return a - normal * 2.0f * Dot(normal, a);
  }

  /// Constructs a vector with the specified components.
  inline Vector3 MakeVector3(const float x, const float y, const float z)
  {
    Vector3 v = { x, y, z };
    return v;
  }
}
