# Luculentus -- Proof of concept spectral path tracer
# Copyright (C) 2012  Ruud van Asseldonk
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CFLAGS += -std=c++11 -flto -Ofast -Wall -Wextra -march=native

SOURCES = Camera.cpp Cie1931.cpp Cie1964.cpp Compound.cpp \
  EmissiveMaterial.cpp GatherUnit.cpp Main.cpp Material.cpp \
  MonteCarloUnit.cpp PlotUnit.cpp Raytracer.cpp Scene.cpp SRgb.cpp \
  Surface.cpp TaskScheduler.cpp TonemapUnit.cpp TraceUnit.cpp \
  UserInterface.cpp
SRC = $(addprefix src/, $(SOURCES))
OBJS = $(addsuffix .o, $(basename $(SRC)))
LIBS = -lstdc++ -lm

all: release

release:
	$(CC) $(CFLAGS) $(SRC) -o luculentus -pthread `pkg-config --cflags gtkmm-3.0` `pkg-config --libs gtkmm-3.0` $(LIBS)

clean:
	/bin/rm -f $(OBJS) luculentus
