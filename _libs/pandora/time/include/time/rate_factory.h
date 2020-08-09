/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstdint>
#include "./rate.h"
#if defined(_CPP_REVISION) && _CPP_REVISION == 14
# define constexpr
#endif

namespace pandora {
  namespace time {
    /// @enum SmpteRate
    /// @brief Smpte standard rates
    enum class SmpteRate : int32_t {
      film_24p      = 2400,
      ntsc_24p      = 2397,
      pal_25p_50i   = 2500,
      pal_50p       = 5000,
      ntsc_30p_60i  = 2997,
      ntsc_60p      = 5994,
      ntsc_non_drop_30p_60i = 3000,
      ntsc_non_drop_60p = 6000,
    };
    
    /// @enum AudioSampleRate
    /// @brief Audio standard sampling rates
    enum class AudioSampleRate : int32_t {
      note_a     = 440,
      audio_8k   = 8000,
      audio_16k  = 16000,
      audio_24k  = 24000,
      audio_32k  = 32000,
      audio_44k  = 44100,
      audio_48k  = 48000,
      audio_96k  = 96000,
      audio_192k = 192000
    };

    // ---

    /// @class RateFactory
    /// @brief Utility to build standard rate values
    class RateFactory {
    public:
      RateFactory() = delete;

      /// @brief Create rate object - Smpte standards
      static constexpr inline Rate fromSmpteRate(SmpteRate type) noexcept {
        switch (type) {
          case SmpteRate::film_24p      : return Rate(24, 1u);
          case SmpteRate::ntsc_24p      : return Rate(24000, 1001u);
          case SmpteRate::pal_25p_50i   : return Rate(25, 1u);
          case SmpteRate::pal_50p       : return Rate(50, 1u);
          case SmpteRate::ntsc_30p_60i  : return Rate(30000, 1001u);
          case SmpteRate::ntsc_60p      : return Rate(60000, 1001u);
          case SmpteRate::ntsc_non_drop_30p_60i: return Rate(30, 1u);
          case SmpteRate::ntsc_non_drop_60p : return Rate(60, 1u);
          default: break;
        }
        return Rate(1, 1u);
      }
      
      /// @brief Create rate object - audio sampling standards
      static constexpr inline Rate fromAudioSampleRate(AudioSampleRate type) noexcept {
        return Rate(static_cast<int32_t>(type), 1u);
      }
    };
    
  }
}
#if defined(_CPP_REVISION) && _CPP_REVISION == 14
# undef constexpr
#endif
