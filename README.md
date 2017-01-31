#PandoraGS - GPU driver for emulator
![GPLv2](https://img.shields.io/badge/license-GPL2-blue.svg)
![v0.3.1](https://img.shields.io/badge/latest-v0.3.1-blue.svg)
![Status](https://img.shields.io/badge/status-not_ready-red.svg)
<br/>
![C++14](https://img.shields.io/badge/language-C++11-lightgrey.svg)
![OpenGL4.1](https://img.shields.io/badge/api-OpenGL_4.1-lightgrey.svg)

PandoraGS is a GPU driver plugin for PlayStation emulators (PSEmu compatible).
#########################################################################

    Author       : Romain Vinders
    Languages    : English, French, Spanish, German (or external file)
    Build        : Visual Studio 2015+
    Requirements : Windows NT 6.0+ (Vista, 7, 8, 10+),
                   Visual C++ Runtimes 2015 (x86),
                   OpenGL 4.1 compatible drivers,
                   256MB VRAM or more

#########################################################################

###Done:
* driver init/open/close/shutdown, low-level commands, read/write access
* frame rate management - FPS counter, frame rate limit, frame skipping
* user input - change profile, toggle config elements, goodies (fast forward, slow motion, ...)
* configuration management - general settings, profile system, game/profile association
* configuration dialog - user-friendly UI with tabs
* multilanguage support and error log system
* debug mode - debug terminal + vram data export
* test utilities - unit testing, rendering primitive testing, rendering demo, dialog launcher

###Currently done:
* refactoring - classes, memory management, FIFO buffer, renderer management, C++14 usage
* refactoring - dialog management - add OS abstraction layer
* testing - reorganize unit testing, improve test tool

###To do:
* processing and rendering of primitives and attributes
* display management - screen resolution, internal resolution, ratio, stretching, cropping, mirroring, ...
* textures/sprites/screen filtering and upscaling
* shader effects (AA, motion blur, bump mapping, color filters, noise, HDR, ...)
* configuration dialog - final settings, profile creation tool
* goodies (visual vibrations, snapshots, cursor, scanlines, pause key, ...)

###To do when complete:
* fix game-specific bugs
* full compatibility with ePSXe, PCSX-R and Zinc
* gnu-linux (debian/ubuntu) port
* mac os (10.9+) port
* support for PGXP
