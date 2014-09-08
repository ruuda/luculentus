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

#include "Raytracer.h"

#include "Constants.h"
#include "TraceUnit.h"
#include "PlotUnit.h"
#include "GatherUnit.h"
#include "TonemapUnit.h"
#include "Compound.h"

using namespace Luculentus;

// You paid for these expensive i7-machines, it would be a shame not to
// use all available cores, wouldn't it? You might want to leave one
// core available to run other programs, and keep your system
// responsive. In that case, set less_threads to true.
bool less_threads = false;
#ifndef _DEBUG
const int Raytracer::numberOfThreads =
  std::max<int>(1, std::thread::hardware_concurrency() - less_threads);
#else
// One thread is easier during debugging
const int Raytracer::numberOfThreads = 1; 
#endif

Raytracer::Raytracer()
  : taskScheduler(numberOfThreads, imageWidth, imageHeight, &scene)
{
  workerThreads = new std::thread[numberOfThreads];

  // Set up a scene to render
  BuildScene();
}

Raytracer::~Raytracer()
{
  delete [] workerThreads;
}

void Raytracer::SetUserInterface(UserInterface* ui)
{
  userInterface = ui;
}

void Raytracer::StartRendering()
{
  continueRendering = true;

  // Start the main render thread, running the RunMain function
  mainThread = std::thread(&Raytracer::RunMain, this);
}

void Raytracer::StopRendering()
{
  // Signal all threads to stop rendering
  continueRendering = false;

  // Then wait until the main thread is done
  // waiting for the worker threads
  mainThread.join();
}

void Raytracer::RunMain()
{
  // Start all worker threads
  for (int i = 0; i < numberOfThreads; i++)
  {
    // Execute the RunWorker method on the worker thread
    workerThreads[i] = std::thread(&Raytracer::RunWorker, this);
  }

  // And then wait for all threads to finish
  for (int i = 0; i < numberOfThreads; i++)
  {
    workerThreads[i].join();
  }
}

void Raytracer::RunWorker()
{
  // There is no task yet, but the task scheduler expects a completed
  // task. Therefore, this worker is done sleeping.
  Task task;
  task.type = Task::Sleep;

  // Until something signals this worker to stop,
  // continue executing tasks.
  while (continueRendering)
  {
    // Ask the task scheduler for a new task
    task = taskScheduler.GetNewTask(task);

    // And execute it
    ExecuteTask(task);
  }
}

void Raytracer::ExecuteTask(const Task task)
{
  // Delegate the task to the correct method
  switch (task.type)
  {
    case Task::Sleep:   ExecuteSleepTask(task);   break;
    case Task::Trace:   ExecuteTraceTask(task);   break;
    case Task::Plot:    ExecutePlotTask(task);    break;
    case Task::Gather:  ExecuteGatherTask(task);  break;
    case Task::Tonemap: ExecuteTonemapTask(task); break;
  }
}

void Raytracer::ExecuteSleepTask(const Task)
{
  // Sleep 100 ms, then the task is done
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // That wasn't too hard
}

void Raytracer::ExecuteTraceTask(const Task task)
{
  // Let the trace unit do all the work, then the task is done
  taskScheduler.traceUnits[task.unit].Render();
}

void Raytracer::ExecutePlotTask(Task task)
{
  // Loop through all trace units that need plotting
  while (!task.otherUnits.empty())
  {
    // Take one trace unit
    int index = task.otherUnits.back();
    task.otherUnits.pop_back();
    auto& traceUnit = taskScheduler.traceUnits[index];

    // And plot it using the correct plot unit
    taskScheduler.plotUnits[task.unit].Plot(traceUnit);
  }
}

void Raytracer::ExecuteGatherTask(Task task)
{
  // Loop through all plot units that need to be gathered
  while (!task.otherUnits.empty())
  {
    // Take one plot unit
    int index = task.otherUnits.back();
    task.otherUnits.pop_back();
    auto& plotUnit = taskScheduler.plotUnits[index];

    // Accumulate the plotted data into the gather unit
    taskScheduler.gatherUnit->Accumulate(plotUnit);

    // And then clear the plot unit,
    // so the data does not get accumulated twice
    plotUnit.Clear();
  }
}

void Raytracer::ExecuteTonemapTask(const Task)
{
  // Delegate tonemapping to the tonemap unit
  taskScheduler.tonemapUnit->Tonemap(*taskScheduler.gatherUnit);

  // And then display the tonemapped image on the screen
  userInterface->DisplayImage(imageWidth, imageHeight,
                              &taskScheduler.tonemapUnit->rgbBuffer[0]);
}

/* Begin Huge Monolithic Scene Initialisation Function */

void Raytracer::BuildScene()
{
  Material* grey  = new DiffuseGreyMaterial(0.8f);
  Material* red   = new DiffuseColouredMaterial(0.9f, 660.0f, 60.0f);
  Material* green = new DiffuseColouredMaterial(0.9f, 550.0f, 40.0f);
  Material* blue  = new DiffuseColouredMaterial(0.5f, 470.0f, 25.0f);
  Material* glossLow  = new GlossyMirrorMaterial(0.1f);
  RefractiveMaterial* glass = new Sf10GlassMaterial();
  SoapBubbleMaterial* soap  = new SoapBubbleMaterial();

  EmissiveMaterial* sunEmissive  = new BlackBodyMaterial(6504.0f, 1.0f);
  EmissiveMaterial* sky1Emissive = new BlackBodyMaterial(7600.0f, 0.6f);
  EmissiveMaterial* sky2Emissive = new BlackBodyMaterial(5000.0f, 0.6f);

  // Sphere in the centre
  const float sunRadius = 5.0f;
  Vector3 sunPosition = {  0.0f,  0.0f,  0.0f };
  Sphere* sunSphere   = new Sphere(sunPosition, sunRadius);
  Object  sun         = { sunSphere, nullptr, sunEmissive };
  scene.objects.push_back(sun);

  // Floor paraboloid
  Vector3 floorNormal   = {  0.0f,  0.0f, -1.0f };
  Vector3 floorPosition = {  0.0f,  0.0f, -sunRadius };
  Paraboloid* floorParaboloid = new Paraboloid(floorNormal, floorPosition, sunRadius * sunRadius);
  Object floor          = { floorParaboloid, grey, nullptr };
  scene.objects.push_back(floor);

  // Floorwall paraboloid (left)
  Vector3 wallLeftNormal   = {  0.0f,  0.0f,  1.0f };
  Vector3 wallLeftPosition = {  1.0f,  0.0f, -sunRadius * sunRadius };
  Paraboloid* wallLeftParaboloid = new Paraboloid(wallLeftNormal, wallLeftPosition, sunRadius * sunRadius);
  Object wallLeft          = { wallLeftParaboloid, green, nullptr };
  scene.objects.push_back(wallLeft);

  // Floorwall paraboloid (right)
  Vector3 wallRightNormal   = {  0.0f,  0.0f,  1.0f };
  Vector3 wallRightPosition = { -1.0f,  0.0f, -sunRadius * sunRadius };
  Paraboloid* wallRightParaboloid = new Paraboloid(wallRightNormal, wallRightPosition, sunRadius * sunRadius);
  Object wallRight          = { wallRightParaboloid, red, nullptr };
  scene.objects.push_back(wallRight);

  // Sky light 1
  const float sky1Radius = 5.0f;
  const float skyHeight = 30.0f;
  Vector3 sky1Position = {  -sunRadius,  0.0f,  skyHeight };
  Circle* sky1Circle   = new Circle(-floorNormal, sky1Position, sky1Radius);
  Object  sky1         = { sky1Circle, nullptr, sky1Emissive };
  scene.objects.push_back(sky1);

  // Sky light 2
  const float sky2Radius = 15.0f;
  Vector3 sky2Position = {  -sunRadius * 0.5f,  sunRadius * 2.0f + sky2Radius,  skyHeight };
  Circle* sky2Circle   = new Circle(-floorNormal, sky2Position, sky2Radius);
  Object  sky2         = { sky2Circle, nullptr, sky2Emissive };
  scene.objects.push_back(sky2);

  // Ceiling plane (for more interesting light)
  Vector3 ceilingPosition = {  0.0f,  0.0f, skyHeight * 2.0f };
  Plane* ceilingPlane     = new Plane(floorNormal, ceilingPosition);
  Object ceiling          = { ceilingPlane, blue, nullptr };
  scene.objects.push_back(ceiling);

  // Spiral sunflower seeds
  const float gamma = static_cast<float>(pi * 2.0 * (1.0 - 1.0 / goldenRatio));
  const float seedSize = 0.8f;
  const float seedScale = 1.5f;
  const int firstSeed = static_cast<int>((sunRadius / seedScale + 1) * (sunRadius / seedScale + 1) + 0.5f);
  const int seeds = 100;
  for (int i = firstSeed; i < firstSeed + seeds; i++)
  {    
    const float phi = static_cast<float>(i) * gamma;
    const float r   = std::sqrt(static_cast<float>(i)) * seedScale;
    Vector3 position =
    { 
      std::cos(phi) * r,
      std::sin(phi) * r,
      (r - sunRadius) * -0.5f
    };
    position = position + sunPosition;
    Sphere* sphere = new Sphere(position, seedSize);
    Material* mat = new DiffuseColouredMaterial(0.9f, static_cast<float>(i - firstSeed) / seeds * 130.0f + 600.0f, 60.0f);
    Object object = { sphere, mat, nullptr };
    scene.objects.push_back(object);
  }

  // Seeds in between
  for (int i = firstSeed; i < firstSeed + seeds; i++)
  {    
    const float phi = (static_cast<float>(i) + 0.5f) * gamma;
    const float r   = std::sqrt(static_cast<float>(i) + 0.5f) * seedScale;
    Vector3 position =
    { 
      std::cos(phi) * r,
      std::sin(phi) * r,
      (r - sunRadius) * -0.25f
    };
    position = position + sunPosition;
    Sphere* sphere = new Sphere(position, seedSize * 0.5f);
    Object object = { sphere, glossLow, nullptr };
    scene.objects.push_back(object);
  }

  // Soap bubbles above
  for (int i = firstSeed / 2; i < firstSeed + seeds; i++)
  {    
    const float phi = -static_cast<float>(i) * gamma;
    const float r   = std::sqrt(static_cast<float>(i)) * seedScale * 1.5f;
    Vector3 position =
    { 
      std::cos(phi) * r,
      std::sin(phi) * r,
      (r - sunRadius) * 1.5f + sunRadius * 2.0f
    };
    position = position + sunPosition;
    Sphere* sphere = new Sphere(position, seedSize * (0.5f + std::sqrt(static_cast<float>(i)) * 0.2f));
    Object object = { sphere, soap, nullptr };
    scene.objects.push_back(object);
  }

  // Prisms along the walls
  const int prisms = 11;
  const float prismAngle = static_cast<float>(pi * 2.0f / prisms);
  const float prismRadius = 17.0f;
  const float prismHeight = 8.0;
  for (int i = 0; i < prisms; i++)
  {
    float phi = static_cast<float>(i) * prismAngle;
    {
      // Get an initial position
      Vector3 position = 
      {
        std::cos(phi) * prismRadius,
        std::sin(phi) * prismRadius,
        0.0f
      };
      Vector3 normal = { 0.0f, 0.0f, -1.0f };
      // Get the normal and intersection with the floor
      Ray ray; ray.origin = position; ray.direction = normal;
      Intersection intersection;
      floorParaboloid->Intersect(ray, intersection);
      normal = -intersection.normal; // Parabola focus is on the other side of the paraboloid
      position = intersection.position + normal * 2.0f;
    
      HexagonalPrism* prism = new HexagonalPrism(MakeHexagonalPrism(normal, position, 3.0f, 1.0f, phi, prismHeight));
      Object object = { prism, glass, nullptr };
      scene.objects.push_back(object);
    }

    // Repeat for second prism
    phi += prismAngle * 0.5f;
    {
      Vector3 position = 
      {
        std::cos(phi) * prismRadius * 1.2f,
        std::sin(phi) * prismRadius * 1.2f,
        0.0f
      };
      Vector3 normal = { 0.0f, 0.0f, -1.0f };
      // Get the normal and intersection with the floor
      Ray ray; ray.origin = position; ray.direction = normal;
      Intersection intersection;
      floorParaboloid->Intersect(ray, intersection);
      normal = -intersection.normal; // Parabola focus is on the other side of the paraboloid
      position = intersection.position + normal * 3.0f;
    
      HexagonalPrism* prism = new HexagonalPrism(MakeHexagonalPrism(
        normal, position, 3.0f, 1.0f, phi + static_cast<float>(pi) * 0.5f, prismHeight * 1.5f));
      Object object = { prism, glass, nullptr };
      scene.objects.push_back(object);
    }
  }

  // Set up the camera function
  scene.GetCameraAtTime = [](const float t) -> Camera
  {
    Camera camera;
    // Orbit around (0, 0, 0) based on the time
    const float phi = static_cast<float>(pi) + static_cast<float>(pi) * 0.01f * t;
    const float alpha = static_cast<float>(pi) * 0.3f - static_cast<float>(pi) * 0.01f * t;
    const float distance = 50.0f - 0.5f * t; // Also zoom in a bit (actually, this is a dolly roll, changing FOV is zooming)

    Vector3 cameraPosition =
    {
      std::cos(alpha) * std::sin(phi) * distance,
      std::cos(alpha) * std::cos(phi) * distance,
      std::sin(alpha) * distance
    };

    camera.position = cameraPosition;
    camera.fieldOfView = static_cast<float>(pi * 0.35f);
    camera.orientation =
      // Compensate for the displacement of the camera by rotating, such that (0, 0, 0) remains fixed in the image
      Rotation(0.0f, 0.0f, -1.0f, static_cast<float>(pi) + phi) *
      // Camera is aimed downward with angle alpha
      Rotation(1.0, 0.0, 0.0, -alpha);
    camera.focalDistance = cameraPosition.Magnitude() * 0.9f;
    camera.depthOfField  = 2.0f; // A slight blur, not too much, but enough to demonstrate the effect
    camera.chromaticAberration = 0.012f; // A subtle amount of chromatic aberration

    return camera;
  };

  // Yes, this function leaks memory.
  // All those objects constructed will never be deleted.
  // Except when the program ends.
  // And all memory is freed anyway.
  // Which is exactly what the destructor would do,
  // but now it is done automatically.
}

/* End Huge Monolithic Scene Initialisation Function */
