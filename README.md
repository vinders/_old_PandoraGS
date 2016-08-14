#########################################################################

    PandoraGS
    PS1 Emulation GPU library
    -----------------------------------------------------------------
    
    Author            : Romain Vinders
    Prog. languages   : C++, GLSL, HLSL
    Supported systems : currently Windows NT (Vista+) (later Debian/OS X?)
    License           : GPLv2

#########################################################################

PandoraGS is a GPU driver plugin for PlayStation emulators (PSEmu Pro specification).

* - driver init/open/close/shutdown, low-level commands, read/write access
* + console primitives management
* + graphics rendering - OpenGL 4.1, DirectX 11.0
* + screen management - resolution, ratio, stretching, mirroring, ...
* + filtering/upscaling, shader effects (AA, motion blur, ...)
* - frame rate management - FPS counter, frame rate limit, frame skipping
* + configuration dialog - user-friendly UI with tabs
* - configuration management - general settings, profile system, game/profile association
* - user input - change profile, toggle config elements, goodies (fast forward, ...)
* - multilanguage support and error log system

(- : done ; + : to do)
