# Simple 3D Viewer

Simple 3D Viewer is meant to be a basic 3D viewer with a simple codebase.

## Features

- Loading models supported by the assimp library,
- Light shader modification,
- Postprocesses (FXAA, grayscale, inversion),
- Model transformation,
- Camera speed adjustment,

## Getting Started

This section will show you how to get the viewer up and running.

### Prerequisites

You will need:

- **CMake** - found at [https://cmake.org/](https://cmake.org/)

- **C++ Compiler** - needs to support at least the **C++20** standard, i.e.
  _MSVC_, _GCC_, _Clang_

> _**Note:**_ _You also need to be able to provide _**CMake**_ a supported
> [generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)._

### Installing

Initialize the vcpkg submodule and update it with the following commands:

```
git submodule init
git submodule update
```

If you are using Visual Studio Code then you can use the CMakePresets.json to
configure and build the project. Otherwise you can issue standard cmake
commands, example of these commands with ninja as the build system generator:

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja
ninja 
./bin/Release/Simple3DViewer
```

To apply clang-format issue the following command:

```
ninja clang-format
```
