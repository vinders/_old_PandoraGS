/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : primitive drawing facade
*******************************************************************************/
#pragma once

#include "../frame_buffer_settings.h"
#include "../memory/video_memory.h"
#include "primitive_common.h"
#include "line_primitive.h"
#include "attribute.h"

#define PRIMITIVE_NUMBER 256 // 0x00 - 0xFF
#define PRIMITIVE_NI NULL    // non-implemented commands
#define PRIMITIVE_NOP NULL   // no-operation command
#define PRIMITIVE_GEOMETRY_FIRST_ID      0x20uL // first geometry primitive ID
#define PRIMITIVE_GEOMETRY_LAST_ID       0x7FuL // last geometry primitive ID
#define PRIMITIVE_LINE_GOURAUD_FIRST_ID  0x50uL // first gouraud-shaded line ID


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


        /// @class PrimitiveFacade
        /// @brief Primitive drawing facade
        class PrimitiveFacade
        {
        private:
            static command::memory::VideoMemory* s_pVramAccess;          ///< VRAM access used by primitives
            static command::FrameBufferSettings* s_pDrawSettingsAccess;  ///< Frame buffer settings used by primitives

        public:
            /// @brief Initialize primitive facade
            /// @param usedVram VRAM to use for primitives creation
            /// @param usedDrawSettings Frame buffer settings to use for primitives creation
            static inline void init(memory::VideoMemory& usedVram, FrameBufferSettings& usedDrawSettings)
            {
                s_pVramAccess = &usedVram;
                s_pDrawSettingsAccess = &usedDrawSettings;
            }

            /// @brief Create and process primitive
            /// @param commandId Command identifier
            /// @param pData Primitive raw data blocks
            static inline void createPrimitive(cmd_block_t commandId, cmd_block_t* pData)
            {
                if (isCommandImplemented(commandId))
                    c_pPrimitiveIndex[commandId].command(pData);
            }


            // -- command specificities -- -------------------------------------

            // @brief Extract command identifier from first raw data block
            /// @param raw command data (first block)
            /// @param outCommandId Destination variable for read command ID
            /// @param outSize Destination variable for read command size
            static inline void readCommandInfo(cmd_block_t raw, cmd_block_t& outCommandId, size_t& outSize)
            {
                cmd_block_t commandId = readCommandId(raw);
                if (isCommandImplemented(commandId))
                {
                    outCommandId = commandId;
                    outSize = c_pPrimitiveIndex[commandId].size;
                }
                else
                {
                    outCommandId = 0;
                    outSize = 0;
                }
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

            /// @brief Check if identified primitive type can be skipped
            /// @param commandId Command identifier
            /// @return Skippable or not
            static inline bool isCommandSkippable(cmd_block_t commandId)
            {
                return (commandId < PRIMITIVE_GEOMETRY_FIRST_ID || commandId > PRIMITIVE_GEOMETRY_LAST_ID);
            }


            // -- poly-line management -- --------------------------------------

            /// @brief Check if max size comes from poly-line or not
            /// @param maxSize Max primitive length
            /// @return Poly-line or not
            static inline bool isPolyline(size_t maxSize)
            {
                return (maxSize == poly_line_common_t::maxSize());
            }

            /// @brief Check if poly-line is potentially endable
            /// @param commandId Command identifier
            /// @param curPos Current position in primitive data
            /// @param maxSize Max primitive length
            /// @return Endable or not
            static inline bool isPolylineEndable(cmd_block_t commandId, size_t curPos, size_t maxSize)
            {
                if (isPolyline(maxSize))
                {
                    if (commandId < PRIMITIVE_LINE_GOURAUD_FIRST_ID) // flat-shaded
                    {
                        return line_fp_t::isLineEndable(curPos);
                    }
                    else // gouraud-shaded
                    {
                        return line_gp_t::isLineEndable(curPos);
                    }
                }
                return false;
            }

            /// @brief Check if poly-line data block contains end code
            /// @param dataBlock Data block to check
            /// @return End code or not
            static inline bool isPolylineEndCode(cmd_block_t dataBlock)
            {
                return poly_line_common_t::isEndCode(dataBlock);
            }


            // -- getters (only for primitives) -- -----------------------------

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
