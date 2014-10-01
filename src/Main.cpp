// Luculentus -- Proof of concept spectral path tracer
// Copyright (C) 2012-2014  Ruud van Asseldonk
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

#include "UserInterface.h"
#include "Raytracer.h"

using namespace Luculentus;

int main(int argc, char** argv)
{
  // Build the UI to display the rendered image.
  UserInterface ui(argc, argv);

  // Create the path tracer itself.
  Raytracer raytracer(ui);

  // Display a black image to start with.
  std::vector<std::uint8_t> blackBuffer(1280 * 720 * 3, 0);
  ui.DisplayImage(1280, 720, blackBuffer);

  // Begin rendering with all threads
  raytracer.StartRendering();

  // Run the UI event loop, it returns when the window is closed.
  ui.Run();

  // And when the UI is closed, stop rendering.
  raytracer.StopRendering();

  return 0;
}
