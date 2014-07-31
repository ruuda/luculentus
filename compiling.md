Compiling Luculentus
====================

For Unix-like platforms, a makefile is provided. For Windows, Visual
Studio 2010 project files are included.


Dependencies
============

First of all, you must make sure that you have the required development
libraries of the dependencies on your system.

gtkmm
-----
Luculentus uses gtkmm for its user interface. gtkmm is a C++ wrapper for
the GTK+ toolkit. On Unix-like platforms, you probably have it installed
already, or you can get it from your favourite package repositories. For
Windows, there is an
[excellent installer](https://wiki.gnome.org/Projects/gtkmm/MSWindows) available.
Both the 32-bit and 64-bit versions are available. Please note that the
provided Visual Studio solution is only configured for 64-bit builds.

On Unix-like platforms, gtkmm 3.0 is required. On Windows, you can use
version 2.22.


Compiling
=========
If you used the gtkmm installer, it will have defined an environment
variable that will be used to find the correct directories automatically.

On Unix-like systems, simply run

    $ make
    
and Luculentus should be compiled. If `g++` is your default compiler,
you can compile with `clang++` using

    $ make CC=clang++
