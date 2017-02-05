/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : primitive drawing facade
*******************************************************************************/
#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include "../frame_buffer_settings.h"
#include "../memory/video_memory.h"
#include "primitive_common.h"
#include "line_primitive.h"

#define PRIMITIVE_NUMBER 256  // 0x00 - 0xFF
#define PRIMITIVE_NI nullptr  // non-implemented commands
#define PRIMITIVE_NOP nullptr // no-operation command
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
        struct index_row_t
        {
            void(*command)(command::cmd_block_t*); // function to call to process primitive
            size_t size; // number of 32-bit blocks
        };

        extern const index_row_t c_pPrimitiveIndex[PRIMITIVE_NUMBER]; ///< Primitive index table


        /// @class PrimitiveFacade
        /// @brief Primitive drawing facade
        class PrimitiveFacade
        {
        private:
            static bool s_isInitialized;                                 ///< References status
            static command::memory::VideoMemory* s_pVramAccess;          ///< VRAM access used by primitives
            static command::FrameBufferSettings* s_pDrawSettingsAccess;  ///< Frame buffer settings used by primitives

        public:
            /// @brief Initialize primitive facade
            /// @param[in] usedVram          VRAM to use for primitives creation
            /// @param[in] usedDrawSettings  Frame buffer settings to use for primitives creation
            static void init(memory::VideoMemory& usedVram, FrameBufferSettings& usedDrawSettings) noexcept
            {
                s_pVramAccess = &usedVram;
                s_pDrawSettingsAccess = &usedDrawSettings;
                s_isInitialized = true;
            }
            /// @brief Close primitive facade
            static void close()
            {
                s_isInitialized = false;
                s_pVramAccess = nullptr;
                s_pDrawSettingsAccess = nullptr;
            }

            /// @brief Create and process primitive
            /// @param[in] commandId  Command identifier
            /// @param[in] pData      Primitive raw data blocks
            static inline void createPrimitive(const command::cmd_block_t commandId, command::cmd_block_t* pData)
            {
                if (isCommandImplemented(commandId) && s_isInitialized)
                    c_pPrimitiveIndex[commandId].command(pData);
            }


            // -- getters (only for primitives) -- -----------------------------

            /// @brief Get VRAM access
            /// @returns VRAM access reference
            static inline command::memory::VideoMemory& getVramAccess() noexcept
            {
                return *s_pVramAccess;
            }

            /// @brief Get VRAM access
            /// @returns VRAM access reference
            static inline command::FrameBufferSettings& getFrameBufferSettings() noexcept
            {
                return *s_pDrawSettingsAccess;
            }


            // -- command specificities -- -------------------------------------

            // @brief Extract command identifier from first raw data block
            /// @param[in] raw            Command data (first block)
            /// @param[out] outCommandId  Destination variable for read command ID
            /// @param[out] outSize       Destination variable for read command size
            static inline void readCommandInfo(const command::cmd_block_t raw, command::cmd_block_t& outCommandId, size_t& outSize) noexcept
            {
                command::cmd_block_t commandId = readCommandId(raw);
                if (isCommandImplemented(commandId))
                {
                    outCommandId = commandId;
                    outSize = c_pPrimitiveIndex[commandId].size;
                }
                else // not implemented
                {
                    outCommandId = 0;
                    outSize = 0;
                }
            }

            /// @brief Extract command identifier from first raw data block
            /// @param[in] raw  Command data (first block)
            /// @returns Command identifier
            static inline command::cmd_block_t readCommandId(const command::cmd_block_t raw) noexcept
            {
                return ((raw >> 24) & 0x0FFuL);
            }

            /// @brief Check if identified primitive type is implemented
            /// @param[in] commandId  Command identifier
            /// @returns Availability
            static inline bool isCommandImplemented(const command::cmd_block_t commandId) noexcept
            {
                return (c_pPrimitiveIndex[commandId].command != PRIMITIVE_NI);
            }

            /// @brief Check if identified primitive type can be skipped
            /// @param[in] commandId  Command identifier
            /// @returns Skippable or not
            static inline bool isCommandSkippable(const command::cmd_block_t commandId) noexcept
            {
                return (commandId < PRIMITIVE_GEOMETRY_FIRST_ID || commandId > PRIMITIVE_GEOMETRY_LAST_ID);
            }


            // -- poly-line management -- --------------------------------------

            /// @brief Check if max size comes from poly-line or not
            /// @param[in] maxSize  Max primitive length
            /// @returns Poly-line or not
            static inline bool isPolyline(const size_t maxSize) noexcept
            {
                return (maxSize == poly_line_common_t::maxSize());
            }

            /// @brief Check if poly-line is potentially endable
            /// @param[in] commandId  Command identifier
            /// @param[in] curPos     Current position in primitive data
            /// @param[in] maxSize    Max primitive length
            /// @returns Endable or not
            static inline bool isPolylineEndable(const command::cmd_block_t commandId, const size_t curPos, const size_t maxSize) noexcept
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
            /// @param[in] dataBlock  Data block to check
            /// @returns End code or not
            static inline bool isPolylineEndCode(const command::cmd_block_t dataBlock) noexcept
            {
                return poly_line_common_t::isEndCode(dataBlock);
            }
        };
    }
}
