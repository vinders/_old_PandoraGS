/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : console output window management
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
        /// @class ConsoleWindow
        /// @brief Console output window management
        class ConsoleWindow : public IWindow
        {
        private:
            bool m_isVisible;         ///< current visibility state
            window_handle_t m_window; ///< managed window handle
            
        public:
            /// @brief Create window manager
            ConsoleWindow() : m_isVisible(false) {}
            /// @brief Destroy window manager
            ~ConsoleWindow()
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
