/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : screen information toolset
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#endif
#include "common.h"
#include "screen.h"
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Set list of available screen resolutions
/// @param[out] listToFill    Empty list to fill
/// @param[in]  preferedResX  Prefered horizontal resolution (if available), for index selection
/// @param[in]  preferedResY  Prefered vertical resolution (if available), for index selection
int32_t Screen::listAvailableResolutions(std::vector<resolution_t>& listToFill, const uint32_t preferedResX, const uint32_t preferedResY)
{
    // prepare list
    listToFill.clear();
    listToFill.reserve(40);
    listToFill.push_back({ 0u, 0u }); // auto-detect

    int32_t curIndex = 0;
    std::unordered_map<uint32_t, int32_t> existingRes; // hash-set, to avoid duplicates (key = YyyyXxxx ; value = index to check prefered res)
    existingRes.reserve(40);

    // read driver resolutions
    #ifdef _WINDOWS
    DEVMODE dv;
    memset(&dv, 0, sizeof(DEVMODE));
    dv.dmSize = sizeof(DEVMODE);
    for (uint32_t dr = 0u; EnumDisplaySettings(NULL, dr, &dv); ++dr)
    {
        addNonExistingResolution(listToFill, dv.dmPelsWidth, dv.dmPelsHeight, existingRes, ++curIndex);
    }
    #else
    //...
    #endif

    // add other 4:3 / 5:4
    addNonExistingResolution(listToFill, 640u,  480u, existingRes, ++curIndex); // VGA
    addNonExistingResolution(listToFill, 800u,  600u, existingRes, ++curIndex); // SVGA
    addNonExistingResolution(listToFill, 1024u, 768u, existingRes, ++curIndex); // XGA
    addNonExistingResolution(listToFill, 1280u, 960u, existingRes, ++curIndex); // SXGA-
    addNonExistingResolution(listToFill, 1280u, 1024u, existingRes, ++curIndex); // SXGA
    // add other 16:9 / 16:10
    addNonExistingResolution(listToFill, 1280u, 720u, existingRes, ++curIndex); // WXGA-H
    addNonExistingResolution(listToFill, 1280u, 800u, existingRes, ++curIndex); // WXGA
    addNonExistingResolution(listToFill, 1366u, 768u, existingRes, ++curIndex); // HD
    addNonExistingResolution(listToFill, 1440u, 900u, existingRes, ++curIndex); // WSXGA
    addNonExistingResolution(listToFill, 1600u, 900u, existingRes, ++curIndex); // HD+
    addNonExistingResolution(listToFill, 1920u, 1080u, existingRes, ++curIndex); // fullHD
    addNonExistingResolution(listToFill, 1920u, 1200u, existingRes, ++curIndex); // WUXGA
    addNonExistingResolution(listToFill, 2560u, 1600u, existingRes, ++curIndex); // WQXGA
    addNonExistingResolution(listToFill, 4096u, 2160u, existingRes, ++curIndex); // 4K

    // select prefered index (if available)
    int32_t selectedIndex = 0;
    if (existingRes.count(formatResolutionInteger(preferedResX, preferedResY)))
        selectedIndex = existingRes[formatResolutionInteger(preferedResX, preferedResY)];
    return selectedIndex;
}


/// @brief Convert resolution formatted string to integers
/// @param[in]  resString  Resolution string
/// @param[out] outX       Horizontal resolution
/// @param[out] outY       Vertical resolution
void Screen::parseResolution(const std::wstring& resString, uint32_t outX, uint32_t outY) noexcept
{
    try
    {
        if (resString.empty() == false)
        {
            size_t pos = resString.find(L" x "s, 1u);
            if (pos != std::wstring::npos)
            {
                std::wstring xStr = resString.substr(0, pos);
                xStr = xStr.substr(xStr.find_first_not_of(L' '));
                outX = std::stoi(xStr);
                std::wstring yStr = resString.substr(resString.find_first_not_of(L' ', pos + 3));
                outY = std::stoi(yStr);
            }
            else
            {
                outX = 0;
                outY = 0;
            }
        }
    }
    catch (...)
    {
        outX = 0;
        outY = 0;
    }
}


/// @brief Convert dimension string to integer
/// @param[in]  resString   Resolution string
/// @param[in]  defaultVal  Default value (if invalid number)
uint32_t Screen::parseDimension(std::wstring resString, const uint32_t defaultVal) noexcept
{
    size_t pos = resString.find_first_not_of(L' ');
    if (pos > 0 && pos != std::wstring::npos)
        resString = resString.substr(pos);
    if (resString.empty() == false)
    {
        try
        {
            int32_t val = std::stoi(resString);
            if (val > 0u)
                return static_cast<uint32_t>(val);
        }
        catch (...) {}
    }
    return defaultVal;
}
