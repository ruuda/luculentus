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

#include <cstdint>
#include <memory>
#include "UserInterface.h"
#include "Raytracer.h"

using namespace Luculentus;

int main(int argc, char** argv)
{
  // Build the UI to display the rendered image
  UserInterface ui(argc, argv);

  // Create the path tracer itself
  Raytracer raytracer;

  // Display a black image to start with
  std::uint8_t* blackBuffer = new std::uint8_t[1280 * 720 * 3];
  std::uninitialized_fill_n(blackBuffer, 0, 1280 * 720 * 3);
  ui.DisplayImage(1280, 720, blackBuffer);

  // Tell the tracer what the UI is, so it can display things
  raytracer.SetUserInterface(&ui);

  // Begin renderin with all threads
  raytracer.StartRendering();

  // Run the UI event loop, it returns when the window is closed
  ui.Run();

  delete blackBuffer;

  // And when the UI is closed, stop rendering
  raytracer.StopRendering();

  return 0;
}
