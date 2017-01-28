/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing primitive - common types
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include "../frame_buffer_settings.h"

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        // -- data types -- ----------------------------------------------------

        /// @enum stp_t
        /// @brief Semi-transparency types
        enum class stp_t : uint8_t // (B=back ; F=forward)
        {
            mean = 0,    // B/2 + F/2
            add = 1,     // B + F
            sub = 2,     // B - F
            addPart = 3  // B + F/4
        };
        /// @enum colordepth_t
        /// @brief Color depth types
        enum class colordepth_t : uint8_t
        {
            clut_4bit = 0,
            clut_8bit = 1,
            rgb_15bit = 2,
            reserved = 3
        };


        // -- primitive sub-units - coordinates -- -----------------------------

        /// @struct coord8_tx_t
        /// @brief 8-bit coordinates + texture attributes (MiscYyXx)
        typedef struct 
        {
            cmd_block_t raw;
            inline cmd_block_t x() ///< X coordinate (Xx)
            {
                return (raw & 0x0FFuL);
            }
            inline cmd_block_t y() ///< Y coordinate (Yy)
            {
                return ((raw >> 8) & 0x0FFuL);
            }
            // Misc - CLUT
            inline cmd_block_t clutX()    { return ((raw >> 12) & 0x3F0uL); } ///< X CLUT index: 0, 16, ...
            inline cmd_block_t clutY()    { return ((raw >> 22) & 0x1FFuL); } ///< Y CLUT index: 0-511
            // Misc - Texture page
            inline cmd_block_t texpageX() { return ((raw >> 10) & 0x3C0uL); }        ///< X base: 0, 64, ...
            inline cmd_block_t texpageY() { return ((raw >> 12) & 0x100uL); }        ///< Y base: 0 or 256
            inline bool isTextureDisabled() { return ((raw & 0x8000000uL) != 0uL); } ///< Texture mode - normal (0) or disable (1) if texture disabling allowed (GP1(09h).bit0==1)
            inline colordepth_t colorDepth() ///< Color depth mode (4-bit, 8-bit, 15-bit)
            {
                cmd_block_t val = ((raw >> 23) & 0x3uL);
                return static_cast<colordepth_t>(val);
            }
            inline stp_t semiTransparency() ///< Semi-transparency mode
            {
                cmd_block_t val = ((raw >> 21) & 0x3uL);
                return static_cast<stp_t>(val);
            }
        } coord8_tx_t;

        /// @struct coord16_t
        /// @brief 16-bit coordinates (YyyyXxxx)
        typedef struct 
        {
            cmd_block_t raw;
            inline cmd_block_t x() ///< X coordinate (Xxxx)
            {
                return (raw & 0x0FFFFuL);
            }
            inline cmd_block_t y() ///< Y coordinate (Yyyy)
            {
                return ((raw >> 16) & 0x0FFFFuL);
            }
        } coord16_t;

        /// @struct rect16_t
        /// @brief 2x-16-bit rectangular area (YyyyXxxx HhhhWwww)
        typedef struct 
        {
            coord16_t pos;  ///< Top-left XY coordinates (YyyyXxxx)
            coord16_t size; ///< Width / height (HhhhWwww)
        } rect16_t;


        // -- primitive sub-units - colors -- ----------------------------------

        /// @struct rgb16_t
        /// @brief 16-bit RGB color value (0000MBGR)
        typedef struct 
        {
            cmd_block_t raw;
            inline bool isMask() ///< Mask bit (m0000000)
            {
                return ((raw >> 15) != 0uL);
            }
            inline cmd_block_t rgb16() ///< RGB 16-bit color (mBbb-bbGg-gggR-rrrr)
            {
                return (raw & 0x0FFFFuL);
            }
            inline cmd_block_t rgb24() ///< RGB 24-bit color (00BbGgRr)
            {
                return (((raw << 9) & 0x0FF0000) | ((raw << 6) & 0x0FF00) | ((raw << 3) & 0x0FF));
            }
        } rgb16_t;

        /// @struct rgb24_t
        /// @brief 24-bit RGB color value (PpBbGgRr)
        typedef struct 
        {
            cmd_block_t raw;
            inline cmd_block_t r() { return (raw & 0x0FFuL); }           ///< Red (Rr)
            inline cmd_block_t g() { return ((raw >> 8) & 0x0FFuL); }    ///< Green (Gg)
            inline cmd_block_t b() { return ((raw >> 16) & 0x0FFuL); }   ///< Blue (Bb)
            inline cmd_block_t pad() { return ((raw >> 24) & 0x0FFuL); } ///< Padding (Pp)
            inline cmd_block_t rgb16() ///< RGB 16-bit color (mBbb-bbGg-gggR-rrrr)
            {
                return (0x8000uL | (((raw >> 9) & 0x7C00uL) | ((raw >> 6) & 0x03E0uL) | (raw >> 3)));
            }
            inline cmd_block_t rgb24() ///< RGB 24-bit color (00BbGgRr)
            {
                return (raw & 0x0FFFFFFuL);
            }
        } rgb24_t;


        // -- primitive sub-units - vertices -- --------------------------------

        /// @brief Flat-shaded vertex (YyyyXxxx)
        typedef coord16_t vertex_f1_t;

        /// @struct vertex_ft1_t
        /// @brief Textured flat-shaded vertex (YyyyXxxx MiscYyXx)
        typedef struct 
        {
            coord16_t coord;     ///< Vertex coordinates
            coord8_tx_t texture; ///< Texture coordinates + misc texture information
            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        } vertex_ft1_t;

        /// @struct vertex_g1_t
        /// @brief Gouraud-shaded vertex (00BbGgRr YyyyXxxx)
        typedef struct 
        {
            rgb24_t color;    ///< Vertex color (RGB)
            coord16_t coord;  ///< Vertex coordinates
            static inline size_t size() { return 2; } ///< Length (32-bit blocks)
        } vertex_g1_t;

        /// @struct vertex_gt1_t
        /// @brief Textured gouraud-shaded vertex (00BbGgRr YyyyXxxx MiscYyXx)
        typedef struct 
        {
            rgb24_t color;       ///< Vertex color (RGB)
            coord16_t coord;     ///< Vertex coordinates
            coord8_tx_t texture; ///< Texture coordinates + misc texture information
            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } vertex_gt1_t;
    }
}
