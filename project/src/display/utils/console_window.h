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
            bool m_isVisible;         ///< Current visibility state
            window_handle_t m_window; ///< Managed window handle
            FILE* m_hfOutput;         ///< Console output stream descriptor
            
        public:
            /// @brief Create window manager
            ConsoleWindow() : m_isVisible(false), m_hfOutput(NULL) {}
            /// @brief Destroy window manager
            ~ConsoleWindow()
            {
                hide();
            }
            
            /// @brief Display window on screen
            /// @param bufferWidth Console buffer width
            /// @param bufferHeight Console buffer height
            /// @param height Window height (zero to keep buffer height)
            virtual void show(uint32_t bufferWidth, uint32_t bufferHeight, int32_t height);

            /// @brief Hide existing window
            virtual void hide();

            /// @brief Clear window content
            virtual void clear();
        };
    }
}
