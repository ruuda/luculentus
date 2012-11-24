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
  struct Intersection
  {
    /// The position at which the intersection occurred.
    Vector3 position;

    /// The surface normal at the intersection.
    Vector3 normal;

    /// The surface tangent at the intersection.
    Vector3 tangent;

    /// Distance between the intersection position and the ray origin.
    float distance;
  };
}
