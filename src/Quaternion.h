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

namespace Luculentus
{
  struct Quaternion;

  Quaternion Conjugate(const Quaternion q);

  struct Quaternion
  {
    public:

      float x, y, z, w;

      inline float MagnitudeSquared() const
      {
        return x * x + y * y + z * z + w * w;
      }

      inline float Magnitude() const
      {
        return std::sqrt(MagnitudeSquared());
      }

      inline void Normalise()
      {
        float magnitude = Magnitude();
        
        // Ignore the 0-quaternion
        if (magnitude == 0.0f) return;

        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
        w /= magnitude;
      }
  };

  inline Quaternion Conjugate(const Quaternion q)
  {
    Quaternion conj = { -q.x, -q.y, -q.z, q.w };
    return conj;
  }

  inline Quaternion operator+(const Quaternion a, const Quaternion b)
  {
    Quaternion sum = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return sum;
  }

  inline Quaternion operator-(const Quaternion a, const Quaternion b)
  {
    Quaternion diff = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return diff;
  }

  inline Quaternion operator-(const Quaternion q)
  {
    Quaternion neg = { -q.x, -q.y, -q.z, -q.w };
    return neg;
  }

  inline Quaternion operator*(const Quaternion q, const float f)
  {
    Quaternion prod = { q.x * f, q.y * f, q.z * f, q.w * f };
    return prod;
  }

  inline Quaternion operator*(const float f, const Quaternion a)
  {
    return a * f;
  }

  inline Quaternion operator/(const Quaternion q, const float f)
  {
    Quaternion quot = { q.x / f, q.y / f, q.z / f, q.w / f };
    return quot;
  }

  inline Quaternion operator*(const Quaternion a, const Quaternion b)
  {
    Quaternion prod =
    {
      a.x * b.w  +  a.w * b.x  +  a.y * b.z  -  a.z * b.y,
      a.w * b.y  -  a.x * b.z  +  a.y * b.w  +  a.z * b.x,
      a.w * b.z  +  a.x * b.y  -  a.y * b.x  +  a.z * b.w,
      a.w * b.w  -  a.x * b.x  -  a.y * b.y  -  a.z * b.z
    };
    return prod;
  }

  /// Rotates the vector with the specified quaternion
  inline Vector3 Rotate(const Vector3 a, const Quaternion q)
  {
    Quaternion p = { a.x, a.y, a.z, 0.0f };
    Quaternion r = q * p * Conjugate(q);
    Vector3 s    = { r.x, r.y, r.z };
    return s;
  }


  inline Quaternion Rotation(const float x, const float y,
                             const float z, const float angle)
  {
    Quaternion q =
    {
      std::sin(angle * 0.5f) * x,
      std::sin(angle * 0.5f) * y,
      std::sin(angle * 0.5f) * z,
      std::cos(angle * 0.5f)
    };
    return q;
  }

  inline Quaternion MakeQuaternion(const float x, const float y,
                                   const float z, const float w)
  {
    Quaternion q = { x, y, z, w };
    return q;
  }
}
