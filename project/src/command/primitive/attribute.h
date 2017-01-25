/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing attribute (area / transparency)
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
        /// @class Attribute
        /// @brief Drawing attribute (area / transparency)
        class Attribute
        {
        public:
            /// @brief Create primitive
            /// @param pData Raw attribute data
            /// @param frameSettings Current frame buffer settings
            static void setAttribute(cmd_block_t* pData, FrameBufferSettings& frameSettings);
        };


        // -- attribute types - frame buffer settings -- ---------------------------

        /// @struct attr_texpage_t
        /// @brief Draw mode / Texture page change
        typedef struct DR_TEXPAGE
        {
            cmd_block_t raw;
            // attributes
            inline cmd_block_t x() { return ((raw << 6) & 0x3C0uL); }            ///< Texture page X base: 0, 64, ...
            inline cmd_block_t y() { return ((raw << 4) & 0x100uL); }            ///< Texture page Y base: 0 or 256
            inline bool isXFlip()    { return ((raw & 0x1000uL) != 0uL); }       ///< Textured rectangle X-flip
            inline bool isYFlip()    { return ((raw & 0x2000uL) != 0uL); }       ///< Textured rectangle Y-flip
            inline bool isDithered() { return ((raw & 0x200uL) != 0uL); }        ///< Dither 24bit to 15bit (0 = off (strip LSBs) ; 1 = enabled)
            inline bool isDrawingAllowed()  { return ((raw & 0x400uL) != 0uL); } ///< Drawing to display area (0 = forbidden ; 1 = allowed)
            inline bool isTextureDisabled() { return ((raw & 0x800uL) != 0uL); } ///< Texture mode - normal (0) or disable (1) if texture disabling allowed (GP1(09h).bit0 == 1)
            inline colordepth_t colorDepth() ///< Color depth mode (4-bit, 8-bit, 15-bit)
            {
                cmd_block_t val = ((raw >> 7) & 0x3uL);
                return (colordepth_t)val;
            }
            inline stp_t semiTransparency() ///< Semi-transparency mode
            {
                cmd_block_t val = ((raw >> 5) & 0x3uL);
                return (stp_t)val;
            }

            static inline size_t size() { return 1; } ///< Length (32-bit blocks)
        } attr_texpage_t;

        /// @struct attr_texwin_t
        /// @brief Texture window change
        typedef struct DR_TEXWIN
        {
            cmd_block_t raw;
            // attributes
            inline cmd_block_t maskX()   { return (raw & 0x1FuL); }         ///< Mask X (8 pixel steps) = manipulated bits
            inline cmd_block_t maskY()   { return ((raw >> 5) & 0x1FuL); }  ///< Mask Y (8 pixel steps)
            inline cmd_block_t offsetX() { return ((raw >> 10) & 0x1FuL); } ///< Offset X (8 pixel steps) = value for these bits
            inline cmd_block_t offsetY() { return ((raw >> 15) & 0x1FuL); } ///< Offset Y (8 pixel steps)
            // - texcoord = (texcoord AND (NOT (mask*8))) OR ((offset AND mask)*8)
            // - Area within texture window is repeated throughout the texture page (repeats not stored, but "read" as if present)
            static inline size_t size() { return 1; } ///< Length (32-bit blocks)
        } attr_texwin_t;

        /// @struct attr_drawarea_t
        /// @brief Drawing area change
        typedef struct DR_AREA
        {
            cmd_block_t raw;
            // attributes
            inline cmd_block_t x() { return (raw & 0x3FFuL); }         ///< X coordinate
            inline cmd_block_t y() { return ((raw >> 10) & 0x3FFuL); } ///< Y coordinate (must be framebuffer height max (e.g. 512) -> check it before using it)

            static inline size_t size() { return 1; } ///< Length (32-bit blocks)
        } attr_drawarea_t;

        /// @struct attr_drawoffset_t
        /// @brief Drawing offset modification
        typedef struct DR_OFFSET
        {
            cmd_block_t raw;
            // attributes
            inline cmd_block_t x() { return (raw & 0x7FFuL); }         ///< X coordinate
            inline cmd_block_t y() { return ((raw >> 11) & 0x7FFuL); } ///< Y coordinate

            static inline size_t size() { return 1; } ///< Length (32-bit blocks)
        } attr_drawoffset_t;

        /// @struct attr_stpmask_t
        /// @brief Semi-transparency bit change
        typedef struct DR_STPMASK
        {
            cmd_block_t raw;
            // attributes
            inline bool isMaskBitForced() { return ((raw & 0x1uL) != 0uL); }  ///< Set mask while drawing (0 = texture with bit 15 ; 1 = force bit15=1)
            // - When bit0 is off, the upper bit of data written to framebuffer is equal to bit15 of the texture color 
            //   (it is set for colors that are marked as "semi-transparent"). For untextured polygons, bit15 is set to zero.
            inline bool isMaskBitChecked() { return ((raw & 0x2uL) != 0uL); } ///< Check mask before drawing (0 = always draw ; 1 = draw if bit15==0)
            // - When bit1 is on, any old pixels in the framebuffer with bit15==1 are write-protected, and cannot be overwritten by rendering commands.
            // - The mask setting affects all rendering commands, as well as CPU-to-VRAM and VRAM-to-VRAM transfer commands (where it acts as for 15bit textures). 
            //   However, Mask does NOT affect the Fill-VRAM command.
            static inline size_t size() { return 1; } ///< Length (32-bit blocks)
        } attr_stpmask_t;


        // -- attribute types - data transfer settings -- --------------------------

        /// @struct img_load_t
        /// @brief Load image (cpu to vram)
        typedef struct DR_LOAD
        {
            cmd_block_t cmd;       ///< Primitive ID
            coord16_t destination; ///< framebuffer destination: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t size;        ///< size (width / height): X = 0..400h ; Y = 0..200h (if more, clipped)
            // - Size=0 is handled as Size=max
            // - Transfer data through DMA
            // - Parameters are clipped to 10bit (X) / 9bit (Y) range, the only special case is that Size=0 is handled as Size=max.
            // - If the Source/Dest starting points plus the width/height value exceed the framebuffer size, wrap to the opposite memory edge.
            // - Affected by the mask settings
            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } img_load_t;

        /// @struct img_store_t
        /// @brief Store image (vram to central memory)
        typedef struct DR_STORE
        {
            cmd_block_t cmd;    ///< Primitive ID
            coord16_t source;   ///< framebuffer source: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t size;     ///< size (width / height): X = 0..400h ; Y = 0..200h (if more, clipped)
            // - Size=0 is handled as Size=max
            // - If the Source/Dest starting points plus the width/height value exceed the framebuffer size, wrap to the opposite memory edge.
            // - Transfer data through DMA or gpuread port
            static inline size_t size() { return 3; } ///< Length (32-bit blocks)
        } img_store_t;

        /// @struct img_move_t
        /// @brief Framebuffer rectangle copy (vram to vram)
        typedef struct DR_MOVE
        {
            cmd_block_t cmd;       ///< Primitive ID
            coord16_t source;      ///< framebuffer source: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t destination; ///< framebuffer destination: X = 0..3FFh ; Y = 0..1FFh (if more, clipped)
            coord16_t size;        ///< size (width / height): X = 0..400h ; Y = 0..200h (if more, clipped)
            // - Size=0 is handled as Size=max
            // - If the Source/Dest starting points plus the width/height value exceed the framebuffer size, wrap to the opposite memory edge
            // - Affected by the mask settings
            static inline size_t size() { return 4; } ///< Length (32-bit blocks)
        } img_move_t;
    }
}
