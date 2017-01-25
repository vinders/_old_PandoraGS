/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing rectangle primitive (tile / sprite)
*******************************************************************************/
#pragma once

#include "i_primitive.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        /// @class RectPrimitive
        /// @brief Drawing rectangle primitive (tile / sprite)
        class RectPrimitive : public IPrimitive
        {
        public:
            /// @brief Create primitive
            /// @param pData Raw primitive data
            /// @param frameSettings Current frame buffer settings
            RectPrimitive(unsigned long* pData, FrameBufferSettings& frameSettings);
            
            /// @brief Process primitive
            /// @param outputBuffer Output buffer for primitive formatted data
            virtual void process(std::vector<float>& outputBuffer);
        };


        // -- primitive units - tiles -- ---------------------------------------

        /// @struct tile_f_t
        /// @brief Tile of any desired size
        typedef struct TILE_F
        {
            rgb24_t color;  ///< Primitive ID + tile color (RGB)
            rect16_t coord; ///< Position (top-left) + size (width/height)

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } tile_f_t;
        
        /// @struct tile_f1_t
        /// @brief 1 x 1 fixed-size tile
        typedef struct TILE_F1
        {
            rgb24_t color; ///< Primitive ID + tile color (RGB)
            coord16_t pos; ///< Position (top-left)

            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        } tile_f1_t;
        
        /// @struct tile_f8_t
        /// @brief 8 x 8 fixed-size tile
        typedef struct TILE_F8
        {
            rgb24_t color; ///< Primitive ID + tile color (RGB)
            coord16_t pos; ///< Position (top-left)

            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        } tile_f8_t;
        
        /// @struct tile_f16_t
        /// @brief 16 x 16 fixed-size tile
        typedef struct TILE_F16
        {
            rgb24_t color; ///< Primitive ID + tile color (RGB)
            coord16_t pos; ///< Position (top-left)

            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        } tile_f16_t;

        /// @struct sprite_f1_t
        /// @brief Fill blank area
        typedef struct DR_FILL
        {
            rgb24_t color;  ///< Primitive ID + rectangle color (RGB)
            coord16_t pos;  ///< top-left coordinates
            coord16_t size; ///< size (width / height)
            // - Horizontally, the filling is done in 16-pixel (32-bytes) units (steps of 10h).
            // - X-size==400h works only indirectly: handled as X-size==0. However, X-size==[3F1h..3FFh] is rounded-up as X-size==400h.
            // - If the Source/Dest starting points plus the width/height value exceed the framebuffer size, wrap to the opposite memory edge.
            // - NOT affected by the mask settings
            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } fill_area_t;


        // -- primitive units - sprites -- -------------------------------------

        /// @struct sprite_f_t
        /// @brief Sprite of any desired size
        typedef struct SPRT_F
        {
            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT
            coord16_t size;      ///< Size (width/height)

            static inline size_t size() { return 4; } ///< Length (32-bit blocks)
        } sprite_f_t;
        
        /// @struct sprite_f1_t
        /// @brief 1 x 1 fixed size texture-mapped sprite
        typedef struct SPRT_F1
        {
            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } sprite_f1_t;
        
        /// @struct sprite_f8_t
        /// @brief 8 x 8 fixed size texture-mapped sprite
        typedef struct SPRT_F8
        {
            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } sprite_f8_t;
        
        /// @struct sprite_f16_t
        /// @brief 16 x 16 fixed size texture-mapped sprite
        typedef struct SPRT_F16
        {
            rgb24_t color;       ///< Primitive ID + tile color (RGB)
            coord16_t pos;       ///< Position (top-left)
            coord8_tx_t texture; ///< Sprite texture coordinates + CLUT

            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } sprite_f16_t;

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
