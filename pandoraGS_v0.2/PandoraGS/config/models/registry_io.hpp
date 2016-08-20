/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   registry_io.hpp
Description : registry IO helper
*******************************************************************************/
#ifndef _REGISTRY_IO_HPP
#define _REGISTRY_IO_HPP
using namespace std;
#include "config_io.h"

#ifdef _WINDOWS 
// -- REGISTRY GETTERS/SETTERS -- ----------------------------------------------
///Advapi32.lib

/// <summary>Read DWORD registry value</summary>
/// <param name="pDest">Destination for read value</param>
/// <param name="pRegKey">Registry key containing value</param>
/// <param name="valName">Value identifier</param>
template<typename T> inline void readRegDword(T* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    DWORD val;
    *pSize = sizeof(DWORD);
    if (RegQueryValueEx(*pRegKey, valName, 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        *pDest = (T)val;
}

/// <summary>Read bool registry value</summary>
inline void readRegBool(bool* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    DWORD val;
    *pSize = sizeof(DWORD);
    if (RegQueryValueEx(*pRegKey, valName, 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        *pDest = (val != 0uL);
}

/// <summary>Read float registry value</summary>
inline void readRegFloat(float* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    DWORD val;
    *pSize = sizeof(DWORD);
    if (RegQueryValueEx(*pRegKey, valName, 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        *pDest = *((float *)(&val));

    if (*pDest == 0.0f)
    {
        std::wstring legacyName = L"Conv";
        legacyName += valName;
        if (RegQueryValueEx(*pRegKey, legacyName.c_str(), 0, pType, (LPBYTE)&val, pSize) == ERROR_SUCCESS)
        {
            if (val > 5uL && (long)val > 0L)
                *pDest = (float)val / 100.0f;
        }
    }
}

/// <summary>Convert registry string to standard string</summary>
inline void convertRegString(std::string* pDest, WCHAR* valBuffer, DWORD* pSize)
{
    //convert from wide char to narrow char array
    if (pSize != NULL && *pSize > 0)
    {
        int len = *pSize;
        valBuffer[len] = L'\0';
        char* ncharBuffer = new char[len];
        char defChar = ' ';
        WideCharToMultiByte(CP_ACP, 0, valBuffer, -1, ncharBuffer, 1 + len, &defChar, NULL);
        *pDest = std::string(ncharBuffer);
        delete[] ncharBuffer;
    }
    else
        *pDest = "";
}
/// <summary>Read string registry value</summary>
inline void readRegString(std::string* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    WCHAR* valBuffer = new WCHAR[CFG_STRING_BUFFER_LENGTH];
    *pSize = CFG_STRING_BUFFER_LENGTH;
    if (RegQueryValueEx(*pRegKey, valName, 0, NULL, (LPBYTE)valBuffer, pSize) == ERROR_SUCCESS)
        convertRegString(pDest, valBuffer, pSize);
    delete[] valBuffer;
}
/// <summary>Read string path registry value</summary>
inline void readRegPath(std::string* pDest, HKEY* pRegKey, LPCWSTR valName, DWORD* pType, DWORD* pSize)
{
    WCHAR* valBuffer = new WCHAR[CFG_PATH_BUFFER_LENGTH];
    *pSize = CFG_PATH_BUFFER_LENGTH;
    if (RegQueryValueEx(*pRegKey, valName, 0, NULL, (LPBYTE)valBuffer, pSize) == ERROR_SUCCESS)
        convertRegString(pDest, valBuffer, pSize);
    delete[] valBuffer;
}

/// <summary>Set float registry value</summary>
/// <param name="pDest">Source value</param>
/// <param name="pRegKey">Registry key containing value</param>
/// <param name="valName">Value identifier</param>
inline void setRegFloat(float source, HKEY* pRegKey, LPCWSTR valName)
{
    DWORD val = *((DWORD *)&source);
    RegSetValueEx(*pRegKey, valName, 0, REG_DWORD, (LPBYTE)&val, sizeof(val));

    std::wstring legacyName = L"Conv";
    legacyName += valName;
    float conv = source * 100.0f;
    val = (DWORD)conv;
    RegSetValueEx(*pRegKey, legacyName.c_str(), 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
}

#endif
#endif
