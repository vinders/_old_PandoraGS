/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : GPU status register
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>

// status bits
#define GPUSTATUS_INIT              0x14802000u // initial values
#define GPUSTATUS_ODDLINES          0x80000000u
#define GPUSTATUS_DMABITS           0x60000000u // 2 bits
#define GPUSTATUS_READYFORCOMMANDS  0x10000000u
#define GPUSTATUS_READYFORVRAM      0x08000000u
#define GPUSTATUS_IDLE              0x04000000u
#define GPUSTATUS_DISPLAYDISABLED   0x00800000u
#define GPUSTATUS_INTERLACED        0x00400000u
#define GPUSTATUS_RGB24             0x00200000u
#define GPUSTATUS_PAL               0x00100000u
#define GPUSTATUS_DOUBLEHEIGHT      0x00080000u
#define GPUSTATUS_WIDTHBITS         0x00070000u // 3 bits
#define GPUSTATUS_MASKENABLED       0x00001000u
#define GPUSTATUS_MASKDRAWN         0x00000800u
#define GPUSTATUS_DRAWINGALLOWED    0x00000400u
#define GPUSTATUS_DITHER            0x00000200u


/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// GPU memory management
    namespace memory
    {
        /// @class StatusRegister
        /// @brief GPU status register
        class StatusRegister
        {
        private:
            static uint32_t s_statusReg;   ///< GPU status register
            static long s_busyEmuSequence; ///< 'GPU busy' emulation hack - sequence value


        public:
            /// @brief Initialize status
            static inline void init() noexcept
            {
                s_statusReg = GPUSTATUS_INIT;
            }


            // -- status getters/setters -- ------------------------------------

            /// @brief Activate specific status bit(s) in status register
            /// @param[in] statusBits  Bits mask to set
            static inline void setStatus(const uint32_t statusBits) noexcept
            {
                s_statusReg |= statusBits;
            }
            /// @brief Remove specific status bit(s) in status register
            /// @param[in] statusBits  Bits mask to remove
            static inline void unsetStatus(const uint32_t statusBits) noexcept
            {
                s_statusReg &= ~statusBits;
            }
            /// @brief Set full status register
            /// @param[in] status  Status register
            static inline void setStatusRegister(const uint32_t status) noexcept
            {
                s_statusReg = status;
            }
            /// @brief Check if status bit is active in status register
            /// @param[in] statusBits  Bit(s) mask
            /// @returns True if at least one bit is active
            static inline bool getStatus(const uint32_t statusBit) noexcept
            {
                return ((s_statusReg & statusBit) != 0);
            }
            /// @brief Get specific status bit(s) from status register
            /// @param[in] statusBits  Bits mask
            /// @returns Bit map
            static inline int32_t getStatusBits(const uint32_t statusBits) noexcept
            {
                return (s_statusReg & statusBits);
            }
            /// @brief Get full status register
            /// @returns Status register
            static inline uint32_t getStatusRegister() noexcept
            {
                return s_statusReg;
            }


            // -- busy/idle sequence -- ----------------------------------------

            /// @brief Init fake busy/idle sequence sequence
            static inline void initFakeBusySequence() noexcept
            {
                s_busyEmuSequence = 4; // set busy/idle sequence
            }

            /// @brief Set fake busy/idle sequence step
            static inline void setFakeBusyStep() noexcept
            {
                // busy/idle sequence (while drawing)
                if (s_busyEmuSequence)
                {
                    --s_busyEmuSequence;
                    if (s_busyEmuSequence & 0x1) // busy if odd number
                        unsetStatus(GPUSTATUS_IDLE | GPUSTATUS_READYFORCOMMANDS);
                    else
                        setStatus(GPUSTATUS_IDLE | GPUSTATUS_READYFORCOMMANDS);
                }
            }
        };
    }
}
