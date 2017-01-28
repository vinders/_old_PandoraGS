/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : image transfer command
*******************************************************************************/
#pragma once

#include "primitive_common.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        // -- attribute types - data transfer settings -- --------------------------

        /// @struct cache_clear_t
        /// @brief Clear texture cache
        struct cache_clear_t
        {
        private:
            cmd_block_t cmd;       ///< Command ID
        public:
            /// @brief Process attribute
            /// @param pData Raw command data pointer
            static void process(cmd_block_t* pData);

            static inline size_t size() { return 1; } ///< Length (32-bit blocks)
        };


        /// @struct img_load_t
        /// @brief Load image (cpu to vram)
        struct img_load_t
        {
            /// @brief Process attribute
            /// @param pData Raw command data pointer
            static void process(cmd_block_t* pData);

            cmd_block_t cmd;       ///< Command ID
            coord16_t destination; ///< frame buffer destination: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t size;        ///< size (width / height): X = 0..400h ; Y = 0..200h (if more, clipped)
            // - Size=0 is handled as Size=max
            // - Transfer data through DMA
            // - Parameters are clipped to 10bit (X) / 9bit (Y) range, the only special case is that Size=0 is handled as Size=max.
            // - If the Source/Dest starting points plus the width/height value exceed the frame buffer size, wrap to the opposite memory edge.
            // - Affected by the mask settings
            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        };


        /// @struct img_store_t
        /// @brief Store image (vram to central memory)
        struct img_store_t
        {
            /// @brief Process attribute
            /// @param pData Raw command data pointer
            static void process(cmd_block_t* pData);

            cmd_block_t cmd;    ///< Command ID
            coord16_t source;   ///< frame buffer source: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t size;     ///< size (width / height): X = 0..400h ; Y = 0..200h (if more, clipped)
            // - Size=0 is handled as Size=max
            // - If the Source/Dest starting points plus the width/height value exceed the frame buffer size, wrap to the opposite memory edge.
            // - Transfer data through DMA or gpuread port
            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        };


        /// @struct img_move_t
        /// @brief Frame buffer rectangle copy (vram to vram)
        struct img_move_t
        {
            /// @brief Process attribute
            /// @param pData Raw command data pointer
            static void process(cmd_block_t* pData);

            cmd_block_t cmd;       ///< Command ID
            coord16_t source;      ///< frame buffer source: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t destination; ///< frame buffer destination: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t size;        ///< size (width / height): X = 0..400h ; Y = 0..200h (if more, clipped)
            // - Size=0 is handled as Size=max
            // - If the Source/Dest starting points plus the width/height value exceed the frame buffer size, wrap to the opposite memory edge
            // - Affected by the mask settings
            static inline size_t size() { return 4; } ///< Length (32-bit blocks)
        };
    }
}
