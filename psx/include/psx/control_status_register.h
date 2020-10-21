/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GPU control/data status register + IO
*******************************************************************************/
#pragma once

#include <cstdint>
#if !defined(_CPP_REVISION) || _CPP_REVISION != 14
# define __static_constexpr static constexpr
#else
# define __static_constexpr
#endif

namespace psx { 
  /// @brief SMPTE video standard type (control status subset)
  enum class VideoStandard : uint32_t {
    ntsc  = 0x00000000u,  ///< 0: NTSC 29.97
    pal   = 0x00100000u   ///< 1: PAL 25
  };
  /// @brief Field type (active lines) for interlacing (control status subset)
  enum class InterlacingField : uint32_t {
    evenScanline = 0x00000000u,  ///< no interlacing, even lines
    oddScanline  = 0x80000000u,  ///< no interlacing, odd lines
    evenField    = 0x00400000u,  ///< 0: even lines
    oddField     = 0x80400000u   ///< 1: odd lines
  };
  /// @brief Color data storage type for textures (control status subset)
  enum class DisplayColorDepth : uint32_t {
    direct15bit  = 0x00000000u,  ///< 0: 15-bit color display
    direct24bit  = 0x00200000u   ///< 1: 24-bit color display
  };
  /// @brief Color data storage type for textures (control status subset)
  enum class TextureColorMode : uint32_t {
    lookupTable4bit  = 0x00000000u, ///< 00: 4-bit lookup table
    lookupTable8bit  = 0x00000080u, ///< 01: 8-bit lookup table
    directColor15bit = 0x00000100u, ///< 10: direct 15-bit
    reserved         = 0x00000180u  ///< 11: reserved (15-bit)
  };
  /// @brief Semi-transparency pixel modes (control status subset)
  /// @note  Pixels: back = pixel read in framebuffer, front = semi-transparent pixel
  enum class SemiTransparencyMode : uint32_t {                        
    mean       = 0x00000000u, ///< 00: 0.5*back + 0.5*front
    add        = 0x00000020u, ///< 01: 1 * back + 1 * front
    subtract   = 0x00000040u, ///< 10: 1 * back - 1 * front
    addQuarter = 0x00000060u  ///< 11: 1 * back + 0.25*front
  };
  /// @brief Direct memory access mode (control status subset)
  enum class DmaMode : uint32_t {
    off             = 0x00000000u,  ///< 00: DMA off (communication through GP0)
    fifoStatusQuery = 0x20000000u,  ///< 01: query to check FIFO status
    cpuToGpu        = 0x40000000u,  ///< 10: DMA CPU -> GPU
    gpuToCpu        = 0x60000000u   ///< 11: DMA GPU -> CPU
  };
  
  // ---
  
  /// @class ControlStatusRegister
  /// @brief GPU control & data exchange status register of PlayStation
  class ControlStatusRegister final {
  public:
    ControlStatusRegister() noexcept { reset(); }
    ControlStatusRegister(const ControlStatusRegister&) = default;
    ControlStatusRegister(ControlStatusRegister&&) = default;
    ControlStatusRegister& operator=(const ControlStatusRegister&) = default;
    ControlStatusRegister& operator=(ControlStatusRegister&&) = default;
    ~ControlStatusRegister() = default;
    
    // -- general operations --
    
    /// @brief Read full register
    constexpr inline uint32_t read() const noexcept { return this->_register; }
    /// @brief Read control register (high bits)
    constexpr inline uint32_t readControlStatus() const noexcept { return (this->_register >> 16); }
    /// @brief Read data exchange register (low bits)
    constexpr inline uint32_t readDataExchangeStatus() const noexcept { return (this->_register & 0xFFFFu); }
    
    /// @brief Reset control/data status register to initial values
    void reset() noexcept;
    /// @brief Write full register
    inline void write(uint32_t status) noexcept { this->_register = status; }
    
    // -- control status --
    
    constexpr uint32_t displayAreaWidth() const noexcept;  ///< Width of display area in frame buffer (pixels)
    constexpr uint32_t displayAreaHeight() const noexcept; ///< Height of display area in frame buffer (pixels)
    constexpr bool isDisplayEnabled() const noexcept;      ///< Display output status (enabled / disabled)
    constexpr DisplayColorDepth displayColorDepth() const noexcept; ///< Color depth of display area in frame buffer
    
    constexpr VideoStandard videoStandard() const noexcept;///< Localization (SMPTE video standard: NTSC / PAL)
    constexpr bool hasVerticalInterlacing() const noexcept;///< Interlacing status (on / off)
    constexpr InterlacingField fieldLines() const noexcept;///< Active lines: current field (interlaced) or scanline (progressive) (even / odd) 
                                                           ///  Note: always 0 during Vblank (vertical retrace / upper/lower screen border)
    
    constexpr bool isIdle() const noexcept;                ///< GPU availability (idle and ready for command word / busy)
    constexpr bool isReadyForDmaCommands() const noexcept; ///< GPU operational status (ready to receive DMA commands / not ready)
    constexpr bool isReadyForVramTransfer() const noexcept;///< GPU VRAM transfer status (ready to send images / not ready)
    constexpr DmaMode dmaMode() const noexcept;            ///< GPU direct memory access mode
    constexpr bool interruptRequest1() const noexcept;     ///< IRQ1 interruption request (on / off)
    void interruptRequest1(bool isEnabled) noexcept;
    
    // -- data exchange status --
    
    constexpr uint32_t texturePageLocationX() const noexcept;        ///< Current texture page horizontal location (pixels)
    constexpr uint32_t texturePageLocationY() const noexcept;        ///< Current texture page vertical location (pixels)
    constexpr TextureColorMode texturePageColors() const noexcept;   ///< Current texture page color mode (direct / looukup table)
    constexpr SemiTransparencyMode semiTransparency() const noexcept;///< Semi-transparency (blending type)
    constexpr bool dithering() const noexcept;                       ///< dither 24-bit->15-bit colors (15-bit output: on/strip-LSB / 24-bit output: on/off)
    
    constexpr bool canDrawToDisplayArea() const noexcept;  ///< drawing to display area (enabled / disabled)
    constexpr bool canDrawToMaskedPixels() const noexcept; ///< drawing to pixels with set mask bit (enabled / disabled)
    constexpr bool applyMaskToDrawnPixels() const noexcept;///< apply mask bit to drawn pixels (on / off)
    constexpr bool canDrawTextures() const noexcept;       ///< drawing with textures (enabled / disabled)
    
  protected:
    enum class StatusBits : uint32_t;
  
    // -- helpers --
  
    template <typename _ResultType, StatusBits _Bit>
    constexpr inline _ResultType _readBit() const noexcept { return static_cast<_ResultType>(this->_register & (uint32_t)_Bit); }
    template <typename _ResultType, uint32_t _Bits>
    constexpr inline _ResultType _readBitmask() const noexcept { return static_cast<_ResultType>(this->_register & _Bits); }

    template <StatusBits _Bit>
    inline void _setBit(bool isActive) noexcept { 
      if (isActive)
        this->_register |= (uint32_t)_Bit;
      else
        this->_register &= ~(uint32_t)_Bit; 
    }

    // -- constants --
    
    __static_constexpr const uint32_t _displayWidths[8] = { 256u, 384u, 320u, 384u, 512u, 384u, 640u, 384u }; // order: width mode value (width0 + width1 bits)
    __static_constexpr const uint32_t _displayHeights[4] = { 240u, 480u, 256u, 512u }; // order: height mode value (interlacing + height bits)
    
  private:
    uint32_t _register{ 0u };
  };
  
  
  // -- private -- -------------------------------------------------------------

// GP1(08h) - Display mode
// The PS1 hardware supports several different modes, the width can be one of: 256, 320, 384, 512 or 640. The height can be 240 or 480(interlaced) for NTSC, PAL supports 256 or 512(interlaced).
// Overscan: if width is 384px: 16px lost because of CRT overscan -> 368px visible (warning: 368 centered in 384 (8 lost on each side))
// Overscan: actual TVs show about 224 lines of the signal, hence the commonly reported 256x224 resolution.
// The 224/264 values are for fullscreen pictures. Many NTSC games display 240 lines (overscan with hidden lines). Many PAL games display only 256 lines (underscan with black borders).

# define _CSREG ControlStatusRegister
# define _CSBITS ControlStatusRegister::StatusBits

  enum class _CSREG::StatusBits : uint32_t {
    // control status
    drawOddLines         = 0x80000000u, ///< Current field drawn (interlaced) or current scanline (progressive) (1: odd lines / 0: even lines)
    directMemoryAccess   = 0x60000000u, ///< direct memory access mode (gpuToCpu/cpuToGpu/off)
    readyForDmaCommands  = 0x10000000u, ///< GPU operational status (1: ready to receive commands / 0: not ready)
    readyForVramTransfer = 0x08000000u, ///< GPU VRAM transfer status (1: ready to send image / 0: not ready)
    gpuAvailable         = 0x04000000u, ///< GPU availability status (1: idle and ready for command word / 0: busy)
    dmaDataRequestState  = 0x02000000u, ///< depends on 'directMemoryAccess' value (off: always 0 / query: FIFO ready (1) or full (0) / toGpu: readyForDmaCommands / toCpu : readyForVramTransfer)
    interruptRequest1    = 0x01000000u, ///< interrupt request IRQ1 (on/off)
    disableDisplay       = 0x00800000u, ///< display status (1: disabled / 0: enabled)
    verticalInterlacing  = 0x00400000u, ///< interlace mode (on/off)
    colorDepth           = 0x00200000u, ///< display area color depth mode (1: 24-bit direct mode / 0: 15-bit direct mode)
    videoStandard        = 0x00100000u, ///< smpte video standard (1: PAL / 0: NTSC)
    displayAreaHeight    = 0x00080000u, ///< display area height mode in framebuffer
    displayAreaWidth0    = 0x00060000u, ///< display area width mode in framebuffer - part 1 (256/320/512/640)
    displayAreaWidth1    = 0x00010000u, ///< display area width mode in framebuffer - part 2 (0: use part1 / 1: use 384)
    displayAreaWidthFull = 0x00070000u, ///< display area width mode in framebuffer - both parts
    
    // data exchange status
    texturesDisabled     = 0x00008000u, ///< Disable textures when drawing (on / off)
    legacyReverseFlag    = 0x00004000u, ///< Reverse flag, only used by first generation GPUs (on / off)
    interlaceField       = 0x00002000u, ///< interlace field (always 1 when 'verticalInterlacing' is off)
    maskEnabled          = 0x00001000u, ///< no drawing to pixels with set mask bit (on/off)
    maskBitDrawn         = 0x00000800u, ///< apply mask bit to drawn pixels (on: force to 1 / off: use bit15 of texture color or set to 0 for untextured polygons)
    drawingDisplayArea   = 0x00000400u, ///< drawing to display area allowed (on/off)
    dithering            = 0x00000200u, ///< dither 24-bit->15-bit (15-bit output: on/strip-LSB / 24-bit output: on/off)
    texturePageColors    = 0x00000180u, ///< lookup table (4-bit/8-bit) or 15-bit colors
    semiTransparency     = 0x00000060u, ///< semi-transparency mode (blending type)
    texturePageBaseY     = 0x00000010u, ///< texture page Y location (to multiply by 256 pixels)
    texturePageBaseX     = 0x0000000Fu  ///< texture page X location (to multiply by 64 pixels)
  };
  
  // -- control status - definitions --

  constexpr inline uint32_t _CSREG::displayAreaWidth() const noexcept { 
    return _displayWidths[(_readBit<uint32_t, _CSBITS::displayAreaWidthFull>() >> 16)]; 
  }
  constexpr inline uint32_t _CSREG::displayAreaHeight() const noexcept { 
    return _displayHeights[(_readBitmask<uint32_t, ((uint32_t)_CSBITS::displayAreaHeight | (uint32_t)_CSBITS::videoStandard)>() >> 19)]; 
  }
  constexpr inline bool _CSREG::isDisplayEnabled() const noexcept              { return (_readBit<uint32_t, _CSBITS::disableDisplay>() == 0u); }
  constexpr inline DisplayColorDepth _CSREG::displayColorDepth() const noexcept{ return _readBit<DisplayColorDepth, _CSBITS::colorDepth>(); }
  
  constexpr inline VideoStandard _CSREG::videoStandard() const noexcept { return _readBit<VideoStandard, _CSBITS::videoStandard>(); }
  constexpr inline bool _CSREG::hasVerticalInterlacing() const noexcept { return _readBit<bool, _CSBITS::verticalInterlacing>(); }
  constexpr inline InterlacingField _CSREG::fieldLines() const noexcept { 
    return _readBitmask<InterlacingField, ((uint32_t)_CSBITS::verticalInterlacing | (uint32_t)_CSBITS::drawOddLines)>(); 
  }
  
  constexpr inline bool _CSREG::isIdle() const noexcept                 { return _readBit<bool, _CSBITS::gpuAvailable>(); }
  constexpr inline bool _CSREG::isReadyForDmaCommands() const noexcept  { return _readBit<bool, _CSBITS::readyForDmaCommands>(); }
  constexpr inline bool _CSREG::isReadyForVramTransfer() const noexcept { return _readBit<bool, _CSBITS::readyForVramTransfer>(); }
  constexpr inline DmaMode _CSREG::dmaMode() const noexcept             { return _readBit<DmaMode, _CSBITS::directMemoryAccess>(); }
  constexpr inline bool _CSREG::interruptRequest1() const noexcept      { return _readBit<bool, _CSBITS::interruptRequest1>(); }
  inline void _CSREG::interruptRequest1(bool isEnabled) noexcept        { _setBit<_CSBITS::interruptRequest1>(isEnabled); }
  
  // -- data exchange status - definitions --
  
  constexpr inline uint32_t _CSREG::texturePageLocationX() const noexcept        { return (_readBit<uint32_t, _CSBITS::texturePageBaseX>() << 6); }
  constexpr inline uint32_t _CSREG::texturePageLocationY() const noexcept        { return (_readBit<uint32_t, _CSBITS::texturePageBaseY>() << 4); }
  constexpr inline TextureColorMode _CSREG::texturePageColors() const noexcept   { return _readBit<TextureColorMode, _CSBITS::texturePageColors>(); }
  constexpr inline SemiTransparencyMode _CSREG::semiTransparency() const noexcept{ return _readBit<SemiTransparencyMode, _CSBITS::semiTransparency>(); }
  constexpr inline bool _CSREG::dithering() const noexcept                       { return _readBit<bool, _CSBITS::dithering>(); }
  
  constexpr inline bool _CSREG::canDrawToDisplayArea() const noexcept  { return _readBit<bool, _CSBITS::drawingDisplayArea>(); }
  constexpr inline bool _CSREG::canDrawToMaskedPixels() const noexcept { return (_readBit<uint32_t, _CSBITS::maskEnabled>() == 0u); }
  constexpr inline bool _CSREG::applyMaskToDrawnPixels() const noexcept{ return _readBit<bool, _CSBITS::maskBitDrawn>(); }
  constexpr inline bool _CSREG::canDrawTextures() const noexcept       { return (_readBit<uint32_t, _CSBITS::texturesDisabled>() == 0u); }
  
  // -- operations - definitions --
  
  inline void _CSREG::reset() noexcept { 
    this->_register = ((uint32_t)_CSBITS::readyForDmaCommands
                     | (uint32_t)_CSBITS::gpuAvailable
                     | (uint32_t)_CSBITS::disableDisplay
                     | (uint32_t)_CSBITS::interlaceField); 
  }
}
#undef _CSREG
#undef _CSBITS
#undef __static_constexpr
