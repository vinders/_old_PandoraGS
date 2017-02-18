/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : screen information toolset
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
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
            /// @struct resolution_t
            /// @brief Resolution value
            struct resolution_t
            {
                uint32_t x; ///< Horizontal
                uint32_t y; ///< Vertical
            };

            /// @class Screen
            /// @brief Screen information toolset
            class Screen
            {
            public:
                /// @brief Set list of available screen resolutions
                /// @param[out] listToFill    Empty list to fill
                /// @param[in]  preferedResX  Prefered horizontal resolution (if available), for index selection
                /// @param[in]  preferedResY  Prefered vertical resolution (if available), for index selection
                static int32_t listAvailableResolutions(std::vector<resolution_t>& listToFill, const uint32_t preferedResX, const uint32_t preferedResY);

                /// @brief Convert resolution formatted string to integers
                /// @param[in]  resString  Resolution string
                /// @param[out] outX       Horizontal resolution
                /// @param[out] outY       Vertical resolution
                static void parseResolution(const std::wstring& resString, uint32_t outX, uint32_t outY) noexcept;
                /// @brief Convert dimension string to integer
                /// @param[in]  resString   Resolution string
                /// @param[in]  defaultVal  Default value (if invalid number)
                static uint32_t Screen::parseDimension(std::wstring resString, const uint32_t defaultVal) noexcept;


            private:
                /// @brief Add resolution string to list and map, if not already there
                /// @param[out] listToFill  List to fill
                /// @param[in] resX  Horizontal resolution
                /// @param[in] resY  Vertical resolution
                /// @param[out] mapToFill   Map to fill
                /// @param[in]  indexValue  Value added in map (if new entry)
                static inline void addNonExistingResolution(std::vector<resolution_t>& listToFill, const uint32_t resX, const uint32_t resY,
                                                            std::unordered_map<uint32_t, int32_t>& mapToFill, const int32_t indexValue)
                {
                    uint32_t buffer = formatResolutionInteger(resX, resY);
                    if (!mapToFill.count(buffer))
                    {
                        listToFill.push_back({ resX, resY });
                        mapToFill[buffer] = indexValue;
                    }
                }

                /// @brief Group horizontal/vertical resolutions in integer (YyyyXxxx)
                /// @param[in] resX  Horizontal resolution
                /// @param[in] resY  Vertical resolution
                /// @returns Formatted integer
                static inline uint32_t formatResolutionInteger(const uint32_t resX, const uint32_t resY)
                {
                    return ((resY << 16) | resX);
                }

            public:
                /// @brief Create formatted resolution string
                /// @param[in] resX  Horizontal resolution
                /// @param[in] resY  Vertical resolution
                /// @returns Formatted string
                static inline std::wstring formatResolutionString(const uint32_t resX, const uint32_t resY)
                {
                    wchar_t pBuffer[15];
                    pBuffer[14] = '\0';
                    #ifdef _WINDOWS
                    _snwprintf_s(pBuffer, sizeof(pBuffer) - 1, L"%5d x %4d", resX, resY);
                    #else
                    //...
                    #endif
                    return std::wstring(pBuffer);
                }
            };
        }
    }
}