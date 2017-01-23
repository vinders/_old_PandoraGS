/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : graphics output window management
*******************************************************************************/
#pragma once

#include "i_window.h"

/// @namespace display
/// Display management
namespace display
{
    /// @namespace display.utils
    /// Display management utilities
    namespace utils
    {
        /// @class DisplayWindow
        /// @brief Graphics output window management
        class DisplayWindow : public IWindow
        {
        private:
            bool m_isVisible;         ///< current visibility state
            window_handle_t m_window; ///< managed window handle
            
        public:
            /// @brief Create window manager
            /// @param window System window handle
            DisplayWindow(window_handle_t window) : m_isVisible(false), m_window(window) {}
            /// @brief Destroy window manager
            ~DisplayWindow()
            {
                hide();
            }
            
            /// @brief Display window on screen
            virtual void show();
            /// @brief Hide existing window
            virtual void hide();
            /// @brief Clear window content
            virtual void clear();
        };
    }
}
