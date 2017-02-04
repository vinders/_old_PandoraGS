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
        /// @see IWindow
        class DisplayWindow : public IWindow
        {
        private:
            bool m_isVisible;           ///< Current visibility state
            display::window_handle_t m_window;   ///< Managed window handle
            window_mode_t m_windowMode; ///< Window mode (fullscreen, fixed, resizable)

            display::window_menu_t m_sourceMenu;   ///< Source window menu handle
            display::window_style_t m_sourceStyle; ///< Source window visual style
            display::rect_t m_sourceSize;          ///< Source window size / position


        public:
            /// @brief Create window manager
            /// @param window System window handle
            DisplayWindow(display::window_handle_t window) : m_isVisible(false), m_window(window), m_sourceMenu(0), m_sourceStyle(0) {}
            /// @brief Destroy window manager
            ~DisplayWindow()
            {
                hide();
            }
            
            /// @brief Display window on screen
            /// @param[in] width       Window width (ignored if fullscreen)
            /// @param[in] height      Window height (ignored if fullscreen)
            /// @param[in] windowMode  Window mode : fullscreen, fixed window, resizable window
            virtual void show(const uint32_t width, const uint32_t height, const int32_t windowMode) override;

            /// @brief Hide existing window
            virtual void hide() override;

            /// @brief Clear window content
            /// @param[in] context      Current device context
            /// @param[in] clearedZone  Cleared zone (rectangle)
            void clear(display::device_handle_t context, display::rect_t& clearedZone);


            // -- getters / setters -- -----------------------------------------
        private:
            /// @brief Prepare visual style for display window
            /// @param[in] windowMode  Window display mode
            void setWindowStyle(const window_mode_t windowMode);

            /// @brief Restore source visual style
            void restoreWindowStyle();

        public:
            /// @brief Get current window size
            /// @param[out] outWidth   Width destination variable
            /// @param[out] outHeight  Height destination variable
            bool getWindowSize(uint32_t& outWidth, uint32_t& outHeight);

            /// @brief Initialize window pixel format and fill window
            /// @param[in] resolution  Display resolution (X, Y)
            /// @param[in] colorMode   Display color mode
            /// @throws runtime_error  Pixel format config failure
            void setPixelFormat(const display::coord_t resolution, const display::window_color_mode_t colorMode);

            /// @brief Read screen display size
            /// @param[out] outWidth   Width destination variable
            /// @param[out] outHeight  Height destination variable
            static void readScreenSize(uint32_t& outWidth, uint32_t& outHeight) noexcept;

            /// @brief Enable or disable screen-saver
            /// @param[in] isEnabled  Enabled/disabled
            static void setScreenSaver(const bool isEnabled);
        };
    }
}
