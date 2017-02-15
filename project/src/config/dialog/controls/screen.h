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
            /// @class Screen
            /// @brief Screen information toolset
            class Screen
            {
            public:
                /// @brief Set formatted list of available screen resolutions
                /// @param[out] listToFill    Empty list to fill
                /// @param[in]  preferedResX  Prefered horizontal resolution (if available), for index selection
                /// @param[in]  preferedResY  Prefered vertical resolution (if available), for index selection
                static int32_t listAvailableResolutions(std::vector<std::wstring>& listToFill, const uint32_t preferedResX, const uint32_t preferedResY);


            private:
                /// @brief Add formatted resolution string to list and map, if not already there
                /// @param[in]  resString   Horizontal resolution
                /// @param[out] listToFill  List to fill
                /// @param[out] mapToFill   Map to fill
                /// @param[in]  mapValue    Value added in map in new entry
                static inline void addNonExistingResolutionString(const std::wstring& resString, std::vector<std::wstring>& listToFill,
                                                                  std::unordered_map<std::wstring, int32_t>& mapToFill, const int32_t mapValue)
                {
                    if (!mapToFill.count(resString))
                    {
                        listToFill.push_back(resString);
                        mapToFill[resString] = mapValue;
                    }
                }

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