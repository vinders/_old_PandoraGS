# Pandora
![C++](https://img.shields.io/badge/language-C++-487.svg)
![glsl](https://img.shields.io/badge/opengl-GLSL_4-487.svg)
![hlsl](https://img.shields.io/badge/direct3d-HLSL_11-487.svg)

Toolbox for my personal image/video/AI/game projects.

## Build status
![Cmake3_14](https://img.shields.io/badge/cmake-3.14-888.svg)
[![codecov](https://codecov.io/gh/vinders/pandora/branch/master/graph/badge.svg)](https://codecov.io/gh/vinders/pandora)
![C++17](https://img.shields.io/badge/c++-17-75b.svg)
![C++14](https://img.shields.io/badge/c++-14-a99dc2.svg)

|           system            |        compiler        |         status         |
|-----------------------------|------------------------|------------------------|
| Windows 7+ <sub>(x86/x86_64)</sub> | ![vs2019](https://img.shields.io/badge/visual_studio-2019+-75b.svg) | [![AppVeyor-VS2019](https://appveyor-matrix-badges.herokuapp.com/repos/vinders/pandora/branch/master/1)](https://ci.appveyor.com/project/vinders/pandora/branch/master) |
|                             | ![vs2017](https://img.shields.io/badge/visual_studio-2017-a99dc2.svg) | [![AppVeyor-VS2017](https://appveyor-matrix-badges.herokuapp.com/repos/vinders/pandora/branch/master/2)](https://ci.appveyor.com/project/vinders/pandora/branch/master) |
|                             | ![mingw7](https://img.shields.io/badge/mingw-gcc_7.1+-75b.svg) | [![AppVeyor-mingw](https://appveyor-matrix-badges.herokuapp.com/repos/vinders/pandora/branch/master/3)](https://ci.appveyor.com/project/vinders/pandora/branch/master) |
| Linux <sub>(x86_64)</sub>          | ![clang7](https://img.shields.io/badge/clang-7.0.0+-75b.svg) | [![Build-Linux-clang](https://travis-matrix-badges.herokuapp.com/repos/vinders/pandora/branches/master/2)](https://travis-ci.org/vinders/pandora) |
|                             | ![gcc7](https://img.shields.io/badge/gcc-7.1+-75b.svg) | [![Build-Linux-gcc](https://travis-matrix-badges.herokuapp.com/repos/vinders/pandora/branches/master/1)](https://travis-ci.org/vinders/pandora) |
|                             | ![gcc5](https://img.shields.io/badge/gcc-5.1+-a99dc2.svg) | [![Build-Linux-gcc5-ndk](https://travis-matrix-badges.herokuapp.com/repos/vinders/pandora/branches/master/3)](https://travis-ci.org/vinders/pandora) |
| Mac OS X <sub>(x86_64)</sub>       | ![clang7](https://img.shields.io/badge/clang-7.0.0+-75b.svg) | [![Build-Mac-clang](https://travis-matrix-badges.herokuapp.com/repos/vinders/pandora/branches/master/4)](https://travis-ci.org/vinders/pandora) |
| iOS 9+ <sub>(arm/arm64)</sub>      | ![clang7](https://img.shields.io/badge/clang-7.0.0+-75b.svg) | [![Build-iOS-ndk](https://travis-matrix-badges.herokuapp.com/repos/vinders/pandora/branches/master/5)](https://travis-ci.org/vinders/pandora) |
| Android 8+ <sub>(arm64)</sub>      | ![android64](https://img.shields.io/badge/android-ndk-a99dc2.svg) | [![Build-Android64-ndk](https://travis-matrix-badges.herokuapp.com/repos/vinders/pandora/branches/master/6)](https://travis-ci.org/vinders/pandora) |

## Building project with Cmake

This project can be opened with any IDE that can natively handle Cmake files, or can be generated with Cmake. Some scripts are available to automatically generate the project for common IDEs ("_scripts" directory).
A few options can be used to build the solution when calling Cmake (or in the CMake settings of the IDE).

Using Cmake: \
**cmake -S [source_dir] -G [generator_name] -A [arch] -B [build_dir] -D[option_name]=[option_value]**

Examples:
> * cmake -S . -G "Visual Studio 16 2019" -A x64 -B _build -DCWORK_TESTS=OFF
> * cmake -S . -G "Unix Makefiles" -B _build -DCWORK_DOCS=ON
> * cmake -S . -G "MinGW Makefiles" -B _build -DCMAKE_C_COMPILER=gcc.exe -DCMAKE_CXX_COMPILER=g++.exe -DCMAKE_MAKE_PROGRAM=mingw32-make.exe -DCWORK_TOOLS=ON

## Cmake options

|    Option    |    Name    |    Value(s)    |    Available for...    |
|--------------|------------|----------------|------------------------|
| Retro-compatibility with older<br>C++ revisions (ex: C++14).<br><sub>*Default: "17" if supported, otherwise "14"*</sub>     | CWORK_CPP_REVISION  | "17"<br>"14"                     | all C++17-compliant compilers  |
| Build separate symbol files, for<br>debuggers such as GDB.<br><sub>*Default: OFF*</sub>            | CWORK_BUILD_SYMBOL_FILES | ON<br>OFF                         | all systems  |
| Include unit tests and integration<br>tests in the built solution.<br><sub>*Default: ON*</sub>     | CWORK_TESTS        | ON<br>OFF                         | all except cross-compilation & ARM  |
| Generate code documentation<br>(doxygen) for the built solution.<br><sub>*Default: OFF*</sub>      | CWORK_DOCS         | ON<br>OFF                         | all except cross-compilation  |
| Include special tools related to the<br>project in the built solution.<br><sub>*Default: ON*</sub> | CWORK_TOOLS        | ON<br>OFF                         | all systems  |
| Generate debugging headers to allow<br>code coverage of the project.<br><sub>*Default: OFF*</sub>  | CWORK_COVERAGE     | ON<br>OFF                         | only linux/unix systems  |
