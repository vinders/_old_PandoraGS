/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing primitive factory
*******************************************************************************/
#pragma once

#include "../frame_buffer_settings.h"
#include "../memory/video_memory.h"
#include "primitive_common.h"
#include "attribute.h"

#define PRIMITIVE_NUMBER 256 // 0x00 - 0xFF
#define PRIMITIVE_NI NULL  // non-implemented commands
#define PRIMITIVE_NOP NULL // no-operation command

/// @namespace command
/// GPU commands management
namespace command
{
    /// @namespace command.primitive
    /// Drawing primitive management
    namespace primitive
    {
        /// @struct index_row_t
        /// @brief Primitive index row
        typedef struct PRIMTABLEROW
        {
            void(*command)(cmd_block_t*); // function to call to process primitive
            size_t size; // number of 32-bit blocks
        } index_row_t;

        extern const index_row_t c_pPrimitiveIndex[PRIMITIVE_NUMBER]; ///< Primitive index table


        /// @class PrimitiveFactory
        /// @brief Drawing primitive factory
        class PrimitiveFactory
        {
        private:
            static command::memory::VideoMemory* s_pVramAccess;          ///< VRAM access used by primitives
            static command::FrameBufferSettings* s_pDrawSettingsAccess;  ///< Frame buffer settings used by primitives

        public:
            /// @brief Create primitive
            /// @param usedVram VRAM to use for primitives creation
            /// @param usedDrawSettings Frame buffer settings to use for primitives creation
            static inline void init(memory::VideoMemory& usedVram, FrameBufferSettings& usedDrawSettings)
            {
                s_pVramAccess = &usedVram;
                s_pDrawSettingsAccess = &usedDrawSettings;
            }

            /// @brief Extract command identifier from first raw data block
            /// @param raw command data (first block)
            /// @return Command identifier
            static inline cmd_block_t readCommandId(cmd_block_t raw)
            {
                return ((raw >> 24) & 0x0FFuL);
            }

            /// @brief Check if identified primitive type is implemented
            /// @param commandId Command identifier
            /// @return Availability
            static inline bool isCommandImplemented(cmd_block_t commandId)
            {
                return (c_pPrimitiveIndex[commandId].command != PRIMITIVE_NI);
            }

            /// @brief Create and process primitive
            /// @param commandId Command identifier
            /// @param pData Primitive raw data blocks
            static inline void createPrimitive(cmd_block_t commandId, cmd_block_t* pData)
            {
                if (isCommandImplemented(commandId))
                    c_pPrimitiveIndex[commandId].command(pData);
            }

            /// @brief Get VRAM access
            /// @return VRAM access reference
            static inline command::memory::VideoMemory& getVramAccess()
            {
                return *s_pVramAccess;
            }

            /// @brief Get VRAM access
            /// @return VRAM access reference
            static inline command::FrameBufferSettings& getFrameBufferSettings()
            {
                return *s_pDrawSettingsAccess;
            }
        };
    }
}
