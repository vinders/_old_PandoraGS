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
#include "../../config.h"
#include "screen.h"
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Set formatted list of available screen resolutions
/// @param[out] listToFill    Empty list to fill
/// @param[in]  preferedResX  Prefered horizontal resolution (if available), for index selection
/// @param[in]  preferedResY  Prefered vertical resolution (if available), for index selection
int32_t Screen::listAvailableResolutions(std::vector<std::wstring>& listToFill, const uint32_t preferedResX, const uint32_t preferedResY)
{
    // prepare list
    if (listToFill.empty() == false)
        listToFill.clear();
    listToFill.reserve(40);
    listToFill.push_back(L"auto"s);

    std::wstring buffer;
    int32_t curIndex = 1;
    std::unordered_map<std::wstring, int32_t> existingRes; // hash-set, to avoid duplicates
    existingRes.reserve(40);

    // read driver resolutions
    #ifdef _WINDOWS
    DEVMODE dv;
    memset(&dv, 0, sizeof(DEVMODE));
    dv.dmSize = sizeof(DEVMODE);
    for (uint32_t dr = 0u; EnumDisplaySettings(NULL, dr, &dv); ++dr)
    {
        buffer = formatResolutionString(dv.dmPelsWidth, dv.dmPelsHeight);
        if (!existingRes.count(buffer))
        {
            listToFill.push_back(buffer);
            existingRes[buffer] = curIndex;
            ++curIndex;
        }
    }
    #else
    //...
    #endif

    // add other 4:3 / 5:4
    addNonExistingResolutionString(L"  640 x  480"s, listToFill, existingRes, curIndex++); // VGA
    addNonExistingResolutionString(L"  800 x  600"s, listToFill, existingRes, curIndex++); // SVGA
    addNonExistingResolutionString(L" 1024 x  768"s, listToFill, existingRes, curIndex++); // XGA
    addNonExistingResolutionString(L" 1280 x  960"s, listToFill, existingRes, curIndex++); // SXGA-
    addNonExistingResolutionString(L" 1280 x 1024"s, listToFill, existingRes, curIndex++); // SXGA
    // add other 16:9 / 16:10
    addNonExistingResolutionString(L" 1280 x  720"s, listToFill, existingRes, curIndex++); // WXGA-H
    addNonExistingResolutionString(L" 1280 x  800"s, listToFill, existingRes, curIndex++); // WXGA
    addNonExistingResolutionString(L" 1366 x  768"s, listToFill, existingRes, curIndex++); // HD
    addNonExistingResolutionString(L" 1440 x  900"s, listToFill, existingRes, curIndex++); // WSXGA
    addNonExistingResolutionString(L" 1600 x  900"s, listToFill, existingRes, curIndex++); // HD+
    addNonExistingResolutionString(L" 1920 x 1080"s, listToFill, existingRes, curIndex++); // fullHD
    addNonExistingResolutionString(L" 1920 x 1200"s, listToFill, existingRes, curIndex++); // WUXGA
    addNonExistingResolutionString(L" 2560 x 1600"s, listToFill, existingRes, curIndex++); // WQXGA
    addNonExistingResolutionString(L" 4096 x 2160"s, listToFill, existingRes, curIndex++); // 4K

    // select prefered index (if available)
    int32_t selectedIndex = 0;
    buffer = formatResolutionString(preferedResX, preferedResY);
    if (existingRes.count(buffer))
        selectedIndex = existingRes[buffer];
    return selectedIndex;
}


/// @brief Add formatted resolution string to list and map, if not already there
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
                outX = RESOLUTION_AUTODETECT;
                outY = RESOLUTION_AUTODETECT;
            }
        }
    }
    catch (...)
    {
        outX = RESOLUTION_AUTODETECT;
        outY = RESOLUTION_AUTODETECT;
    }
}


/// @brief Add formatted resolution string to list and map, if not already there
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
            if (val > 320u)
                return static_cast<uint32_t>(val);
        }
        catch (...) {}
    }
    return defaultVal;
}
