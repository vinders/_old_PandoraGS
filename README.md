#PandoraGS - GPU driver for emulator
PandoraGS is a GPU driver plugin for PlayStation emulators (PSEmu Pro specification).
#########################################################################

    Author            : Romain Vinders
    Prog. languages   : C++, GLSL, HLSL
    Supported systems : currently Windows NT (Vista+) (later Debian/OS X?)
    License           : GPLv2

#########################################################################
/!\ Development halted until mid-september 2016.

###Done:
* driver init/open/close/shutdown, low-level commands, read/write access
* frame rate management - FPS counter, frame rate limit, frame skipping
* user input - change profile, toggle config elements, goodies (fast forward, ...)
* configuration management - general settings, profile system, game/profile association
* multilanguage support and error log system
* testing utilities - unit testing, dialog box launcher

###Partly done:
* configuration dialog - user-friendly UI with tabs

###To do (2016):
* psx primitives management
* basic graphics rendering (OpenGL)
* display management - screen resolution, internal resolution, ratio
* textures/sprites/screen filtering/upscaling
* testing utility - psx vram viewer / editor

###To do (2017):
* enhanced graphics rendering (OpenGL, later Direct3D)
* display management - stretching, cropping, mirroring, ...
* shader effects (AA, motion blur, bump mapping, color filters, noise, HDR, ...)
* debug mode (wireframe, polygons colored by order, polygons colored by precision)
* goodies (visual vibrations, snapshots, cursor, scanlines, ...)
* linux / os x port
