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

#include <cstdint>
#include <Windows.h>

/// @class PandoraGS
/// @brief PandoraGS library - entry point
class PandoraGS
{
private:
    static HINSTANCE s_hInstance; ///< Executed instance handle

public:
    /// @brief Initialize instance handle
    /// @param[in] hModule  Module instance handle
    static void setInstance(const HANDLE* hModule) noexcept;

    /// @brief Get main instance handle
    /// @returns Instance handle
    static inline HINSTANCE getInstance() noexcept
    {
        return s_hInstance;
    }
};

#else
/// @class PandoraGS
/// @brief PandoraGS library - entry point
class PandoraGS
{
public:
	/// @brief Get main instance handle
	/// @returns Instance handle
	static inline int32_t getInstance() noexcept
	{
		return 0;//...
	}
};

#endif
