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
        /// @enum window_mode_t
        /// @brief Window display mode
        enum class window_mode_t : int32_t
        {
            fullscreen = 0,
            fixed = 1,
            resizable = 2
        };
        #define WINDOW_MODE_LENGTH 3


        /// @class DisplayWindow
        /// @brief Graphics output window management
        class DisplayWindow : public IWindow
        {
        private:
            bool m_isVisible;           ///< Current visibility state
            window_handle_t m_window;   ///< Managed window handle
            window_mode_t m_windowMode; ///< Window mode (fullscreen, fixed, resizable)

            window_menu_t m_sourceMenu;   ///< Source window menu handle
            window_style_t m_sourceStyle; ///< Source window visual style
            rect_t m_sourceSize;          ///< Source window size / position


        public:
            /// @brief Create window manager
            /// @param window System window handle
            DisplayWindow(window_handle_t window) : m_isVisible(false), m_window(window), m_sourceMenu(0), m_sourceStyle(0) {}
            /// @brief Destroy window manager
            ~DisplayWindow()
            {
                hide();
            }
            
            /// @brief Display window on screen
            /// @param width Window width (ignored if fullscreen)
            /// @param height Window height (ignored if fullscreen)
            /// @param windowMode Window mode : fullscreen, fixed window, resizable window
            virtual void show(uint32_t width, uint32_t height, int32_t windowMode);

            /// @brief Hide existing window
            virtual void hide();

            /// @brief Clear window content
            /// @param context Current device context
            /// @param clearedZone Cleared zone (rectangle)
            virtual void clear(device_handle_t context, rect_t& clearedZone);


            // -- getters / setters -- -----------------------------------------
        private:
            /// @brief Prepare visual style for display window
            /// @param outWidth Width destination variable
            void setWindowStyle(int32_t windowMode);

            /// @brief Restore source visual style
            void restoreWindowStyle();

        public:
            /// @brief Get current window size
            /// @param outWidth Width destination variable
            /// @param outHeight Height destination variable
            bool getWindowSize(uint32_t& outWidth, uint32_t& outHeight);

            /// @brief Initialize window pixel format and fill window
            /// @param resolution Display resolution (X, Y)
            /// @param colorMode Display color mode
            bool setPixelFormat(display::coord_t resolution, display::window_color_mode_t colorMode);

            /// @brief Read screen display size
            /// @param outWidth Width destination variable
            /// @param outHeight Height destination variable
            static void readScreenSize(uint32_t& outWidth, uint32_t& outHeight);

            /// @brief Enable or disable screen-saver
            /// @param isEnabled Enabled/disabled
            static void setScreenSaver(bool isEnabled);
        };
    }
}
