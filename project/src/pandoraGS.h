/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : GPU library entry point
*******************************************************************************/
#pragma once
#ifdef _WINDOWS

#include <Windows.h>

/// @class PandoraGS
/// @brief PandoraGS library - entry point
class PandoraGS
{
private:
    static HINSTANCE s_hInstance; ///< Executed instance handle

public:
    /// @brief Initialize instance handle
    /// @param hModule Module handle
    static void setInstance(HANDLE* hModule);

    /// @brief Get main instance handle
    /// @return Instance handle
    static inline HINSTANCE getInstance()
    {
        return s_hInstance;
    }
};

#endif
