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

#pragma once

#include <atomic>
#include <thread>
#include "UserInterface.h"
#include "Scene.h"
#include "TaskScheduler.h"

namespace Luculentus
{
  class Raytracer
  {
    public:

      /// Creates a new raytracer
      Raytracer(UserInterface& ui);

      /// Starts rendering on separate threads
      void StartRendering();

      // Waits for all rendering tasks to finish, and then stops
      void StopRendering();

    private:

      /// Width of the rendered image
      static const int imageWidth = 1280;

      /// Height of the rendered image
      static const int imageHeight = 720;

      /// The aspect ratio of the rendered image
      static const float aspectRatio;

      /// Number of worker threads
      static const int numberOfThreads;

      /// Whether to not stop rendering
      std::atomic<bool> continueRendering;

      /// The thread that starts the workers and then waits for them to
      /// finish.
      std::thread mainThread;

      /// The threads that execute the tasks
      std::vector<std::thread> workerThreads;

      /// The TaskScheduler responsible for dividing work across threads.
      TaskScheduler taskScheduler;
      
      /// The UI which displays the render result (not owned)
      UserInterface& userInterface;

      /// The scene which will be rendered
      Scene scene;

      /// Method executed on the main thread
      void RunMain();

      /// Method executed by worker threads
      void RunWorker();

      /// Runs one of the specialised task execution methods,
      /// based on the task type.
      void ExecuteTask(const Task task);

      /// Handles the 'Sleep' task, sleeps 100 ms.
      void ExecuteSleepTask(const Task task);

      /// Executes a 'Trace' task.
      void ExecuteTraceTask(const Task task);

      /// Executes a 'Plot' task.
      void ExecutePlotTask(Task task);

      /// Executes a 'Gather' task.
      void ExecuteGatherTask(Task task);

      /// Executes a 'Tonemap' task, and displays the result in the UI.
      void ExecuteTonemapTask(const Task task);
   };

  /// Initializes the scene with objects.
  Scene BuildScene();
}
