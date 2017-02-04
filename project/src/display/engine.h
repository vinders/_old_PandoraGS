/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : rendering engine - graphics API management
*******************************************************************************/
#pragma once

#include "../vendor/opengl.h" // openGL includes
#include "utils/display_window.h"

/// @namespace display
/// Display management
namespace display
{
    /// @class Engine
    /// @brief Rendering engine - graphics API management
    class Engine
    {
    private:
        // engine status
        static bool s_isInitialized; ///< Rendering engine initialization status
        static GLuint s_programId;   ///< Rendering pipeline program identifier

        // window management
        static utils::DisplayWindow* s_pWindowManager; ///< Main window
        static device_handle_t s_windowDeviceContext;  ///< Window device context
        #ifdef _WINDOWS
        static HGLRC s_openGlRenderContext; ///< API rendering context
        #else
        static GLFWwindow* s_openGlRenderContext; ///< API window context
        #endif


    public:
        /// @brief Create and initialize display window
        /// @param[in] window  Parent window handle
        /// @throws runtime_error  Window creation failure
        static void createDisplayWindow(window_handle_t window);

        /// @brief Close display window and restore menu
        static void closeDisplayWindow();

        /// @brief Render current frame
        static void render();

    private:
        // -- rendering API management -- --------------------------------------

        /// @brief Initialize rendering API
        /// @throws runtime_error API init failure
        static void initGL();

        /// @brief Cleanup and close rendering API
        static void closeGL();

        /// @brief Load/reload rendering pipeline
        static void loadPipeline();
    };
}
