/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : console output window management
*******************************************************************************/
#pragma once

#include <cstdio>
#include <cstdlib>
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
        /// @see IWindow
        class ConsoleWindow : public IWindow
        {
        private:
            bool m_isVisible;         ///< Current visibility state
            window_handle_t m_window; ///< Managed window handle
            FILE* m_hfOutput;         ///< Console output stream descriptor
            
        public:
            /// @brief Create window manager
            ConsoleWindow() : m_isVisible(false), m_hfOutput(nullptr) {}
            /// @brief Destroy window manager
            ~ConsoleWindow()
            {
                hide();
            }
            
            /// @brief Display window on screen
            /// @param[in] bufferWidth   Console buffer width
            /// @param[in] bufferHeight  Console buffer height
            /// @param[in] height        Window height (zero to keep same as buffer height)
            virtual void show(const uint32_t bufferWidth, const uint32_t bufferHeight, const int32_t height) override;

            /// @brief Hide existing window
            virtual void hide() override;

            /// @brief Clear window content
            void clear();
        };
    }
}
