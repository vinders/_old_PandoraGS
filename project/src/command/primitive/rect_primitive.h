/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : rectangle primitive (tile / sprite)
*******************************************************************************/
#pragma once

#include "primitive_common.h"
#pragma pack(push, 4)

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        // -- primitive units - tiles -- ---------------------------------------

        /// @struct fill_area_t
        /// @brief Fill blank area
        struct fill_area_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color;   ///< Primitive ID + rectangle color (RGB)
            coord16_t pos;   ///< top-left coordinates
            coord16_t range; ///< size (width / height)
            // - Horizontally, the filling is done in 16-pixel (32-bytes) units (steps of 10h).
            // - X-size==400h works only indirectly: handled as X-size==0. However, X-size==[3F1h..3FFh] is rounded-up as X-size==400h.
            // - If the Source/Dest starting points plus the width/height value exceed the framebuffer size, wrap to the opposite memory edge.
            // - NOT affected by the mask settings
            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        };


        /// @struct tile_f_t
        /// @brief Tile of any desired size
        struct tile_f_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color;  ///< Primitive ID + tile color (RGB)
            rect16_t coord; ///< Position (top-left) + size (width/height)
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        };
        

        /// @struct tile_f1_t
        /// @brief 1 x 1 fixed-size tile
        struct tile_f1_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color; ///< Primitive ID + tile color (RGB)
            coord16_t pos; ///< Position (top-left)
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        };
        

        /// @struct tile_f8_t
        /// @brief 8 x 8 fixed-size tile
        struct tile_f8_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color; ///< Primitive ID + tile color (RGB)
            coord16_t pos; ///< Position (top-left)
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        };
        

        /// @struct tile_f16_t
        /// @brief 16 x 16 fixed-size tile
        struct tile_f16_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color; ///< Primitive ID + tile color (RGB)
            coord16_t pos; ///< Position (top-left)
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        };


        // -- primitive units - sprites -- -------------------------------------

        /// @struct sprite_f_t
        /// @brief Sprite of any desired size
        struct sprite_f_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT
            coord16_t range;     ///< Size (width/height)
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 4; } ///< Length (32-bit blocks)
        };
        

        /// @struct sprite_f1_t
        /// @brief 1 x 1 fixed size texture-mapped sprite
        struct sprite_f1_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        };
        

        /// @struct sprite_f8_t
        /// @brief 8 x 8 fixed size texture-mapped sprite
        struct sprite_f8_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        };
        

        /// @struct sprite_f16_t
        /// @brief 16 x 16 fixed size texture-mapped sprite
        struct sprite_f16_t
        {
            /// @brief Process primitive
            /// @param[in] pData  Raw primitive data pointer
            static void process(command::cmd_block_t* pData);

            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT
            // rendering information
            inline bool isOpaque()  { return ((color.raw & 0x2000000) == 0uL); }
            inline bool isBlended() { return ((color.raw & 0x1000000) == 0uL); }

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        };


        /*
        Size Restriction: The maximum distance between two vertices is 1023 horizontally, and 511 vertically.
          Polygons and lines that are exceeding that dimensions are NOT rendered.
          If portions of the polygon/line/rectangle are located outside of the drawing area, then the hardware renders only the portion that is inside of the drawing area.

        Caution: For untextured graphics, 8bit RGB values of FFh are brightest.
          However, for texture blending, 8bit values of 80h are brightest (values 81h..FFh make textures brighter (up to twice as bright) as than they were originially stored in memory.
          Of course the results can't exceed the maximum brightness: the 5bit values written to the framebuffer are saturated to max 1Fh.

        Texpage Attribute
          Unust be set separately for sprites (unlike polygons): through the texture page command (0xE1 -> primitive_attr.h).

        Clut Attribute (Color Lookup Table)
          Specifies the location of the CLUT data within VRAM (only for 4bit/8bit textures).
        */
    }
}
#pragma pack(pop)
