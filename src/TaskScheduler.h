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

#include <ctime>
#include <memory>
#include <mutex>
#include <queue>
#include "GatherUnit.h"
#include "PlotUnit.h"
#include "Task.h"
#include "TonemapUnit.h"
#include "TraceUnit.h"

namespace Luculentus
{
  class Scene;
  class UserInterface;

  /// Handles splitting the workload across threads
  class TaskScheduler
  {
    private:

      /// The indices of all TraceUnits which are available for tracing
      /// rays.
      std::queue<int> availableTraceUnits;
      
      /// The indices of all TraceUnits which have MappedPhotons that
      /// must be plotted, before the TraceUnit can be used again.
      std::queue<int> doneTraceUnits;

      /// The indices of all PlotUnits which are available for plotting
      /// MappedPhotons.
      std::queue<int> availablePlotUnits;

      /// The indices of all PlotUnits which have a screen that must be
      /// accumulated, before the PlotUnit can be used again.
      std::queue<int> donePlotUnits;

      /// Whether the GatherUnit is not used at the moment.
      bool gatherUnitAvailable;

      /// Whether the TonemapUnit is not used at the moment.
      bool tonemapUnitAvailable;

      /// Whether a new gather task has been executed since the last
      /// time a tonemapping task was executed
      bool imageChanged;

      /// The last time the image was tonemapped (and displayed)
      std::time_t lastTonemapTime;

      /// The time at which rendering started. Used to measure performance.
      std::time_t startTime;

      /// The number of completed trace batches. Used to measure performance.
      unsigned int completedTraces;

      /// A mutex that ensures only one thread can
      /// access the task scheduler at a given instant.
      std::mutex mutex;

      /// The interval at which tonemapping happens, in seconds
      const static int tonemappingInterval = 10;

    public:

      /// The number of TraceUnits to use
      /// (not all of them have to be active simultaneously).
      size_t numberOfTraceUnits;

      /// The number of PlotUnits to use
      /// (not all of them have to be active simultaneously).
      size_t numberOfPlotUnits;

      /// An array of all TraceUnits in the tracer.
      std::vector<TraceUnit> traceUnits;

      /// An array of all PlotUnits in the tracer.
      std::vector<PlotUnit> plotUnits;

      /// The single GatherUnit.
      std::unique_ptr<GatherUnit>  gatherUnit;

      /// The single TonemapUnit.
      std::unique_ptr<TonemapUnit> tonemapUnit;

      /// Creates a new task scheduler, that will render the specified
      /// scene to a canvas of specified size.
      TaskScheduler(const int numberOfThreads, const int width,
                    const int height, const Scene& scene);

      /// Notifies the task scheduler that a task is complete.
      /// The task scheduler will find some more work to do,
      /// and return it to the caller.
      /// This method is thread-safe.
      Task GetNewTask(const Task completedTask);

    private:

      /// Creates a new 'Sleep' task.
      Task CreateSleepTask();

      /// Creates a new 'Trace' task.
      Task CreateTraceTask();

      /// Creates a new 'Plot' task that plots some TraceUnits which are
      /// done.
      Task CreatePlotTask();

      /// Creates a new 'Gather' task that gathers some PlotUnits which
      /// are done.
      Task CreateGatherTask();

      /// Creates a new 'Tonemap' task.
      Task CreateTonemapTask();
      
      /// Makes resources used by the task available again.
      void CompleteTask(const Task completeTask);

      /// Makes resourced used by a 'Trace' task available again.
      void CompleteTraceTask(const Task completedTask);

      /// Makes resourced used by a 'Plot' task available again.
      void CompletePlotTask(Task completedTask);

      /// Makes resourced used by a 'Gather' task available again.
      void CompleteGatherTask(Task completedTask);

      /// Makes resourced used by a 'Tonemap' task available again.
      void CompleteTonemapTask();
  };
}
