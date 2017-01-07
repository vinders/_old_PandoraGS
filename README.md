#PandoraGS - GPU driver for emulator
![GPLv2](https://img.shields.io/badge/license-GPL2-blue.svg)
![v0.2.2](https://img.shields.io/badge/latest-v0.2.2-blue.svg)
![Status](https://img.shields.io/badge/status-not_ready-red.svg)
##Description
PandoraGS is a GPU driver plugin for PlayStation emulators using the PSEmu Pro specification.
#########################################################################

    Author            : Romain Vinders
    Prog. languages   : C++, OpenGL 4.1, GLSL
    Supported systems : Windows NT (Vista, 7, 8, 10+)

#########################################################################

###Done:
* driver init/open/close/shutdown, low-level commands, read/write access
* frame rate management - FPS counter, frame rate limit, frame skipping
* user input - change profile, toggle config elements, goodies (fast forward, slow motion, ...)
* configuration management - general settings, profile system, game/profile association
* configuration dialog - user-friendly UI with tabs
* multilanguage support and error log system
* debug mode - debug terminal + vram data export
* testing utilities - unit testing, PS1 primitives testing, dialog box launcher

###To do (2017):
* graphics rendering (OpenGL)
* display management - screen resolution, internal resolution, ratio, stretching, cropping, mirroring, ...
* processing and rendering of psx primitives
* textures/sprites/screen filtering/upscaling
* shader effects (AA, motion blur, bump mapping, color filters, noise, HDR, ...)
* configuration dialog - final settings, profile creation tool
* goodies (visual vibrations, snapshots, cursor, scanlines, ...)
* user input - pause/freeze game
* linux / os x port
