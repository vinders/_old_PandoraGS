/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : mouse manager
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include "common.h"

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
            /// @class Button
            /// @brief Dialog button toolset
            class Mouse
            {
            private:
                bool m_isReady;    ///< Mutual exclusion
                int32_t m_accumulator; ///< Accumulator for wheel delta (until enough to scroll)
                #if _DIALOGAPI == DIALOGAPI_WIN32
                DWORD m_previousTime;  ///< Latest action time
                UINT m_linesPerWheelDelta;
                #else
                uint32_t m_previousTime;
                #endif

            public:
                /// @brief Create mouse manager
                /// @param[in] nPage  Current scrolling page
                Mouse(const uint32_t nPage);

                /// @brief Get vertical scrolling value for a mouse wheel event
                /// @param[in] delta  Mouse wheel delta value
                /// @param[in] nPage  Current scrolling page
                /// @returns Scroll value
                int32_t getVerticalWheelScroll(const int32_t delta, uint32_t nPage);
            };
        }
    }
}