/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab button for tab control
*******************************************************************************/
#pragma once

#include <cstdint>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
			#if _DIALOGAPI == DIALOGAPI_WIN32
			/// @struct draw_button_event_args_t
			/// @brief Button drawing event arguments
			struct draw_button_event_args_t
			{
				HWND window;
				HDC context;
				bool isSelected;
				bool isHover;
			};
			#else
			typedef int32_t draw_button_event_args_t; ///< Button drawing event arguments
			#endif


            /// @class TabButton
            /// @brief Tab button for tab control
            class TabButton
            {
			private:
				std::string& m_title;   ///< Button title text (reference)
				uint32_t m_iconIndex; ///< Button icon index

			public:
				/// @brief Create tab button
				/// @param[in] title  Tab name
				/// @param[in] icon   Icon index
				TabButton(std::string& title, const uint32_t icon) : m_title(title), m_iconIndex(icon) {}

				/// @brief Trigger control drawing
				/// @param[in] args  Event arguments
				void invalidate(draw_button_event_args_t& args);
            };
        }
    }
}