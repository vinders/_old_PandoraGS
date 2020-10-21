/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : Field rate calculations + reference frequency constants
*******************************************************************************/
#pragma once

#include <cstdint>
#include "./control_status_register.h"

namespace psx { 
  // -- constants --
  
  constexpr inline double r3000aCpuFrequency() noexcept { return 33868800.0; }      ///< Clock frequency of PlayStation CPU R3000a (Hz)
  constexpr inline double ntscLineDisplayRate() noexcept{ return 9000000.0/572.0; } ///< NTSC display rate for one line (Hz)
  constexpr inline double palLineDisplayRate() noexcept { return 15625.0; }         ///< PAL display rate for one line (Hz)
  
  
  // -- field rate calculations --
  
  /// @brief Get theoretical SMPTE standard field rate
  constexpr inline double smpteFieldRate(VideoStandard standard) const noexcept { 
    return (standard == VideoStandard::ntsc) ? 60000.0/1001.0 : 50.0; // NTSC: 59.94006 / PAL: 50.0
  }
  
  /// @brief Get PlayStation field rate, based on CPU clock
  constexpr inline double cpuFieldRate(VideoStandard standard, bool isInterlaced) const noexcept { 
    return (standard == VideoStandard::ntsc)
           ? ((isInterlaced) ? r3000aCpuFrequency()/565031.25 : r3000aCpuFrequency()/566107.50)  // NTSC: 59.94146i / 59.82751p
           : ((isInterlaced) ? r3000aCpuFrequency()/677343.75 : r3000aCpuFrequency()/680595.00); // PAL: 50.00238i / 49.76352p
  }
  
  /// @brief Get PlayStation field rate, based on GPU clock
  constexpr inline double gpuFieldRate(VideoStandard standard, bool isInterlaced) const noexcept { 
    return (standard == VideoStandard::ntsc)
           ? ((isInterlaced) ? ntscLineDisplayRate()/262.5 : ntscLineDisplayRate()/263.0) // NTSC: 59.94006i / 59,82611p
           : ((isInterlaced) ? palLineDisplayRate()/312.5 : palLineDisplayRate()/314.0);  // PAL: 50.0i / 49,76115p
  }
}
