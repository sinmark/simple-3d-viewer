# Simple C++ Template

A quick C++ template for modern CMake projects.

This repo started of as a copy of modern-cpp-template by Filip Dutescu. I
stripped it down for it to be easier for someone to understand and modify. It is
mainly focused around vcpkg, CMake, GoogleTest and Visual Studio Code.

## Features

- Modern **CMake** configuration and project,

- An example of a **Clang-Format** config,

- **Static analyzers** integration, with _Clang-Tidy_ and _Cppcheck_, the former
  being the default option,

- **Unit testing** support, through _GoogleTest_ (with an option to enable
  _GoogleMock_) and _Catch2_,

- **Code coverage**, enabled by using the `ENABLE_CODE_COVERAGE` option, through
  _Codecov_ CI integration,

- **Package manager support**, _Vcpkg_

- **Permissive license** to allow you to integrate it as easily as possible. The
  template is licensed under the [Unlicense](https://unlicense.org/),

- Options to build as a header-only library or executable, not just a static or
  shared library.

- **Ccache** integration, for speeding up rebuild times

- **CMakePresets** support,

## Getting Started

These instructions will enable you to get the template up and running on your
local machine for development and testing purposes.

### Prerequisites

This project is meant to be only a template, thus versions of the software used
can be change to better suit the needs of the developer(s). If you wish to use
the template _as-is_, meaning using the versions recommended here, then you will
need:

- **CMake v3.15+** - found at [https://cmake.org/](https://cmake.org/)

- **C++ Compiler** - needs to support at least the **C++17** standard, i.e.
  _MSVC_, _GCC_, _Clang_

> _**Note:**_ _You also need to be able to provide _**CMake**_ a supported
> [generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)._

### Installing

It is fairly easy to install the project, all you need to do is clone if from
Github or generate a new repo from it.

After finishing getting a copy of the project, create a new folder in the
`include/` folder, with the name of your project. Edit
`cmake/SourcesAndHeaders.cmake` to add your files.

Initialize the vcpkg submodule and update it with the following commands:

```
git submodule init
git submodule update
```

The project has been tested the most with Visual Studio Code, so this is the
editor I recommend.

More options that you can set for the project can be found in the
[`cmake/StandardSettings.cmake` file](cmake/StandardSettings.cmake). For certain
options additional configuration may be needed in their respective `*.cmake`
files.

You can also build and run the executables and test with standard CMake
commands.

## License

This project is licensed under the [Unlicense](https://unlicense.org/) - see the
[LICENSE](LICENSE) file for details
