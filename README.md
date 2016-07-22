#########################################################################

    PandoraGS
    PS1 Emulation GPU library
    -----------------------------------------------------------------
    
    Author            : Romain Vinders
    Prog. languages   : C++, GLSL, HLSL
    Supported systems : currently Windows NT (Vista+)
    License           : GPLv2

#########################################################################

PandoraGS is a GPU driver plugin for PlayStation emulators (PSEmu Pro specification).

- driver init/open/close/shutdown, low-level commands, read/write access
- graphics rendering - Direct3D WARP, OpenGL 4.1, DirectX 11.0
- screen management - resolution, ratio, stretching, mirroring, ...
- software/shader filtering/upscaling, shader effects (AA, motion blur, ...)
- frame rate management - FPS counter, frame rate limit, frame skipping
- configuration dialog - user-friendly UI with tab pages
- configuration management - general settings, profile system, game/profile association
- user input - change profile, toggle config elements, goodies (fast forward, ...)
- multilanguage support
- error log system
