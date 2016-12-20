#PandoraGS - GPU driver for emulator
PandoraGS is a GPU driver plugin for PlayStation emulators (PSEmu Pro specification).
#########################################################################

    Author            : Romain Vinders
    Prog. languages   : C++, GLSL
    Supported systems : Windows NT (Vista, 7, 8, 10+)
    License           : GPLv2

#########################################################################

###Done:
* driver init/open/close/shutdown, low-level commands, read/write access
* frame rate management - FPS counter, frame rate limit, frame skipping
* user input - change profile, toggle config elements, goodies (fast forward, ...)
* configuration management - general settings, profile system, game/profile association
* configuration dialog - user-friendly UI with tabs
* multilanguage support and error log system
* testing utilities - unit testing, dialog box launcher

###To do (2016):
* testing utilities - psx vram hex viewer / editor, psx primitive test tool

###To do (2017):
* graphics rendering (OpenGL)
* display management - screen resolution, internal resolution, ratio, stretching, cropping, mirroring, ...
* processing and rendering of psx primitives
* textures/sprites/screen filtering/upscaling
* shader effects (AA, motion blur, bump mapping, color filters, noise, HDR, ...)
* configuration dialog - final settings, profile creation tool
* debug mode (wireframe, polygons colored by order, polygons colored by precision)
* goodies (visual vibrations, snapshots, cursor, scanlines, ...)
* linux / os x port
