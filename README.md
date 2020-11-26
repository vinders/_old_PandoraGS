# PandoraGS - GPU driver for emulator
![GPLv2](https://img.shields.io/badge/license-GPL2-blue.svg)
![Status](https://img.shields.io/badge/status-not_ready-red.svg)
![C++14](https://img.shields.io/badge/language-C++14-lightgrey.svg)

PandoraGS is a GPU driver plugin for PlayStation emulators (PSEmu compatible).

                   
### Development halted:
Due to real-life problems (renovating my house, for example), the development of this project has been halted for several years.
In the meantime, I've evolved professionally and learned lots of new techs. 

### Project rebirth:
This project will be restarted from scratch, using the things I've learned (test-driven dev, continuous integration, advanced testing and benchmarks, portable coding, CMake, C++17, Python scripts, gamma correction...).

It will most likely rely on the toolbox libraries I'm currently working on (see my 'pandora_toolbox' repo). It'll probably start as soon as I'm done with the portability of the DisplayMonitor/Window/WindowStyle features I'm working on, and with a basic Direct3D11 & OpenGL4 pipeline. 

This will imply a few difference with the original project:
* portability: it will work on Windows, but also Linux and Mac OS.
* testability/coverage: the code will be organized in a a way that allows a maximum testability.
* continuous integration: CI allows non-regression tests, but also validates the code on different systems/compilers/setups.
* optimization: the code will make use of modern optimizations (C++17, intrinsics, compute shaders...).
* multiple renderers: on Windows, both Direct3D and OpenGL will be supported. Maybe Vulkan too someday (but that one is much harder to learn).
* responsive and portable UI: instead of coding everything with the win32 API, I'll use a good lightweight and portable UI lib (such as Nuklear or ImgUI).
* accuracy: the emulation will be as accurate as possible (but will also provide improvements: subprecision, video filters...)
* goodies: such as a utility to preview video filters through the plugin pipeline.

Since this old repo is polluted with old code and binary files, I'll probably create a new repository (also to cleanup history and lighten the .git dir).
