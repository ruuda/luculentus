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

#pragma once
#include <gtkmm.h>
#include <cstdint>

namespace Luculentus
{
  class UserInterface
  {
    public:
      
      /// Builds the user interface, requires the command-line
      /// parameters to be passed along.
      UserInterface(int argc, char** argv);

      /// Executes UI logic, and runs the application
      void Run();

      /// Displays pixel buffer in the user interface
      void DisplayImage(int width, int height, std::uint8_t* data);

    private:

      /// The main GTK object
      Gtk::Main kit;

      /// The main window
      Gtk::Window window;

      /// The vertically stacked main container
      Gtk::VBox mainBox;

      /// The image that displays the render result
      Gtk::Image resultImage;

      /// The raw byte contents of the render result
      Glib::RefPtr<Gdk::Pixbuf> resultImageBuffer;

      /// A dispatcher to keep the UI on one thread
      Glib::Dispatcher dispatcher;

      /// Displays the image stored in resultImageBuffer in the UI
      void DisplayImage();
  };
}
