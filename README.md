# Simple 3D Viewer

Simple 3D Viewer is meant to be a basic 3D viewer with a simple codebase.

## Features

- Loading models supported by the assimp library,
- Light shader modification,
- Postprocesses (FXAA, grayscale, inversion),
- Model transformation,
- Camera speed adjustment,

## Getting Started

This section is about building the viewer.

### Prerequisites

You will need:

- **CMake v3.15+** - found at [https://cmake.org/](https://cmake.org/)

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

Then start use standard cmake commands to configure the project and build it.
The CMakePresets.json has the required arguments for cmake invocation.
