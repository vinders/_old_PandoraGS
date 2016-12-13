/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   status_register.h
Description : status register and status information
*******************************************************************************/
#ifndef _STATUS_REGISTER_H
#define _STATUS_REGISTER_H
#include "globals.h"
#include <cstddef>
#include <cstdint>
#include <string>

// status bits masks
#define GPUSTATUS_INIT              0x14802000 // initial values
#define GPUSTATUS_ODDLINES          0x80000000
#define GPUSTATUS_DMABITS           0x60000000 // 2 bits
#define GPUSTATUS_READYFORCOMMANDS  0x10000000
#define GPUSTATUS_READYFORVRAM      0x08000000
#define GPUSTATUS_IDLE              0x04000000
#define GPUSTATUS_DISPLAYDISABLED   0x00800000
#define GPUSTATUS_INTERLACED        0x00400000
#define GPUSTATUS_RGB24             0x00200000
#define GPUSTATUS_PAL               0x00100000
#define GPUSTATUS_DOUBLEHEIGHT      0x00080000
#define GPUSTATUS_WIDTHBITS         0x00070000 // 3 bits
#define GPUSTATUS_MASKENABLED       0x00001000
#define GPUSTATUS_MASKDRAWN         0x00000800
#define GPUSTATUS_DRAWINGALLOWED    0x00000400
#define GPUSTATUS_DITHER            0x00000200


// Status register and status information
class StatusRegister
{
private:
    // emulated gpu status
    static int32_t s_statusReg;       // GPU status register
    static std::string s_gameId;      // game executable ID
    static long s_selectedSaveSlot;   // selected save-state slot

    
public:
    /// <summary>Initialize status</summary>
    static void init();


    // -- GETTERS / SETTERS -- -------------------------------------------------

    /// <summary>Activate specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to set</param>
    static inline void setStatus(int32_t statusBits)
    {
        s_statusReg |= statusBits;
    }
    /// <summary>Remove specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to remove</param>
    static inline void unsetStatus(int32_t statusBits)
    {
        s_statusReg &= ~statusBits;
    }
    /// <summary>Check if status bit is active in status register</summary>
    /// <param name="statusBits">Bit(s) mask</param>
    /// <returns>True if at least one bit is active</returns>
    static inline bool getStatus(int32_t statusBit)
    {
        return ((s_statusReg & statusBit) != 0);
    }
    /// <summary>Get specific status bit(s) from status register</summary>
    /// <param name="statusBits">Bits mask</param>
    /// <returns>Bit map</returns>
    static inline int32_t getStatusBits(int32_t statusBits)
    {
        return (s_statusReg & statusBits);
    }
    /// <summary>Get full status register</summary>
    /// <returns>Status register</returns>
    static inline int32_t getStatus()
    {
        return s_statusReg;
    }

    /// <summary>Set game executable identifier</summary>
    /// <param name="pGameId">Game ID</param>
    static inline void setGameId(char* pGameId)
    {
        s_gameId = (pGameId != NULL) ? std::string(pGameId) : std::string("");
    }
    /// <summary>Get game executable identifier</summary>
    /// <returns>Game ID</returns>
    static inline std::string getGameId()
    {
        return s_gameId;
    }

    /// <summary>Set save-state slot index</summary>
    /// <param name="slotIndex">Slot index</param>
    static inline void setSaveSlot(long slotIndex)
    {
        s_selectedSaveSlot = slotIndex;
    }
    /// <summary>Get save-state slot index</summary>
    /// <returns>Slot index</returns>
    static inline long getSaveSlot()
    {
        return s_selectedSaveSlot;
    }
};

#endif
