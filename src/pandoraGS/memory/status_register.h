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
#include <cstddef>
#include <cstdint>
#include <string>

// status bits masks
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


// Status register and status information
class StatusRegister
{
private:
    // emulated gpu status
    static uint32_t s_statusReg;  // GPU status register
    static std::string s_gameId;  // game executable ID

    
public:
    /// <summary>Initialize status</summary>
    static inline void init()
    {
        s_statusReg = GPUSTATUS_INIT;
    }


    // -- GETTERS / SETTERS -- -------------------------------------------------

    /// <summary>Activate specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to set</param>
    static inline void setStatus(uint32_t statusBits)
    {
        s_statusReg |= statusBits;
    }
    /// <summary>Remove specific status bit(s) in status register</summary>
    /// <param name="statusBits">Bits mask to remove</param>
    static inline void unsetStatus(uint32_t statusBits)
    {
        s_statusReg &= ~statusBits;
    }
    /// <summary>Set full status register</summary>
    /// <param name="status">Status register</param>
    static inline void setStatusRegister(uint32_t status)
    {
        s_statusReg = status;
    }
    /// <summary>Check if status bit is active in status register</summary>
    /// <param name="statusBits">Bit(s) mask</param>
    /// <returns>True if at least one bit is active</returns>
    static inline bool getStatus(uint32_t statusBit)
    {
        return ((s_statusReg & statusBit) != 0);
    }
    /// <summary>Get specific status bit(s) from status register</summary>
    /// <param name="statusBits">Bits mask</param>
    /// <returns>Bit map</returns>
    static inline int32_t getStatusBits(uint32_t statusBits)
    {
        return (s_statusReg & statusBits);
    }
    /// <summary>Get full status register</summary>
    /// <returns>Status register</returns>
    static inline uint32_t getStatusRegister()
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
};

#endif
