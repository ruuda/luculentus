Compiling Luculentus
====================

For Windows, Visual Studio 2010 project files are included. For
Unix-like platforms, autoconf or cmake will be provided.


Dependencies
============

First of all, you must make sure that you have the required development
libraries of the dependencies on your system.

Boost
-----
[Boost](http://www.boost.org) is an extension to the Standard Template
Library. On Unix-like platforms, Boost will be present in the package
repositories. On Windows, you need to download the source from
[boost.org](http://www.boost.org/). You can then either compile Boost
yourself, or get prebuilt binaries. [Teeks99](http://boost.teeks99.com/)
has excellent binaries for Visual Studio.

gtkmm
-----
Luculentus uses gtkmm for its user interface. gtkmm is a C++ wrapper for
the GTK+ toolkit. On Unix-like platforms, you probably have it installed
already, or you can get it from your favourite package repositories. For
Windows, there is an
[excellent installer](https://live.gnome.org/gtkmm/MSWindows) available.
Both the 32-bit and 64-bit versions are available. Please note that the
provided Visual Studio solution is only configured for 64-bit builds.


Compiling
=========
For Visual Studio, if you have not done so already, you might need to
add the Boost include directories and linker directories to your search
paths. If you used the gtkmm installer, it will have defined an
environment variable that will be used to find the correct directories
automatically.