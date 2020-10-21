/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
--------------------------------------------------------------------------------
Description : GPU display controller: status + commands handler
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include "./primitives/param_reader.h"
#include "./control_status_register.h"

namespace psx { 
  /// @brief GPU information identifier for queries
  enum class GpuInfoType : uint32_t {
    textureWindow  = 0x2u, ///< Read texture window infos at GP0(0xE2) (20 bit, MSBs to 0)
    drawAreaStart  = 0x3u, ///< Read draw area top-left location at GP0(0xE3) (20 bit, MSBs to 0)
    drawAreaEnd    = 0x4u,///< Read draw area bottom-right location at GP0(0xE4) (20 bit, MSBs to 0)
    drawOffset     = 0x5u,      ///< Read draw offset at GP0(0xE5) (22 bit)
    gpuVersion     = 0x7u,      ///< GPU version (0: early 160-pin GPU / 1: arcade prototype / 2: usual 208-pin GPU)
    lightgunStatus = 0x8u,  ///< Lightgun detection on some arcades (0: no lightgun / 1 : detected)
    biosAddress    = 0xFu      ///< unknown: BIOS address?
  };
  
  // ---

  /// @class DisplayController
  /// @brief GPU display controller: display status management, display control commands (GP0 + GP1), display rendering commands (GP0)
  class DisplayController final {
  public:
    DisplayController() = default;
    DisplayController(const DisplayController&) = default;
    DisplayController(DisplayController&&) = default;
    DisplayController& operator=(const DisplayController&) = default;
    DisplayController& operator=(DisplayController&&) = default;
    ~DisplayController() = default;

    inline ControlStatusRegister& getStatusRegister() noexcept { return this->_statusRegister; }
    inline const ControlStatusRegister& getStatusRegister() const noexcept { return this->_statusRegister; }
    constexpr inline bool isTextureDisablingAllowed() const noexcept { return this->_isTextureDisablingAllowed; }
    
    
    // -- buffered display/rendering commands (GP0) --
    
    void writeDisplayCommand(uint32_t* data, int32_t length) noexcept;
   
  
    // -- direct display control commands (GP1) --
    
    /// @brief Process display control commands, directly passed to the GPU (no FIFO)
    inline void writeControlStatus(uint32_t params) {
      uint32_t commandType = primitives::ParamReader::toGpuCommandId(params);
      switch (commandType) {
        case 0x00: resetGpu(); break;
        case 0x01: clearFifo(); break;
        case 0x02: ackIrq1(); break;
        case 0x03: enableDisplay((params & 0x1u) == 0u); break;
        case 0x04: setDmaMode(static_cast<DmaMode>((params & 0x3u) << 29)); break;
        case 0x05: setDisplayAreaOrigin((params & 0x3FFu), ((params >> 10) & 0x1FFu)); break;
        case 0x06: setHorizontalDisplayRange((params & 0xFFFu), ((params >> 12) & 0xFFFu)); break;
        case 0x07: setVerticalDisplayRange((params & 0x3FFu), ((params >> 10) & 0x3FFu)); break;
        case 0x08: setDisplayMode(params); break;
        case 0x10: requestGpuInfo(static_cast<GpuInfoType>(params & 0x0Fu)); break;
        case 0x09: allowTextureDisabling(params & 0x1u); break;
        case 0x20: debugTexturesLegacy(params); break;
        default: {
          if ((commandType & 0xF0u) == 0x10u) // GPU info mirrors
            requestGpuInfo(static_cast<GpuInfoType>(params & 0x0Fu));
          else if (commandType >= 0x40u) // other commands mirrors
            writeControlStatus(params - 0x40000000u);
          //else
            //syslog(LogLevel::standard, LogCategory::warning, __FILE_NAME__, __LINE__ "writeControlStatus: GP1(%d): unknown command type", commandType);
          break;
        }
      }
    }
    
    void resetGpu() {
      clearFifo();
      ackIrq1();
      enableDisplay(false);
      setDmaMode(DmaMode::off);
      setDisplayAreaOrigin(0u, 0u);
      setHorizontalDisplayRange(0x200u, 0x200u + 2560);
      setVerticalDisplayRange(0x10u, 0x10u + 240u);
      setDisplayMode(0x1u); //320x240 NTSC
      //GP0(E1h..E6h); //rendering attributes (0)
      //GPUSTAT becomes 14802000h
      //has no effect on GP1(09h)
    }
    void clearFifo() noexcept {
      // cancel pending commands
    }
    inline void ackIrq1() noexcept {
      _statusRegister.interruptRequest1(false);
    }
    void enableDisplay(bool isEnabled) {
      
    }
    void setDmaMode(DmaMode mode) {
      
    }
    void setDisplayAreaOrigin(uint32_t x, uint32_t y) {
      
    }
    void setHorizontalDisplayRange(uint32_t x1, uint32_t x2) { 
      // NTSC: usual centering: X1=260h, and Y1/Y2=88h+/-N
      // PAL: many games use weird centering -> option: use auto-centering instead: X1=260h, and Y1/Y2=A3h+/-N
    }
    void setVerticalDisplayRange(uint32_t y1, uint32_t y2) {
      // NTSC: usual centering: X1=260h, and Y1/Y2=88h+/-N
      // PAL: many games use weird centering -> option: use auto-centering instead: X1=260h, and Y1/Y2=A3h+/-N
    }
    void setDisplayMode(uint32_t params) {
      
    }
    void requestGpuInfo(GpuInfoType id) noexcept {
      switch (id) {
        case GpuInfoType::textureWindow: break;
        case GpuInfoType::drawAreaStart: break;
        case GpuInfoType::drawAreaEnd:   break;
        case GpuInfoType::drawOffset:    break;
        case GpuInfoType::gpuVersion:     _dataRequestResult = 2u; break;
        case GpuInfoType::lightgunStatus: _dataRequestResult = 0u; break;
        case GpuInfoType::biosAddress:    _dataRequestResult = 0xBFC03720u; break;
        default: break; // other: keep old value unchanged
      }
    }
    void allowTextureDisabling(bool isAllowed) {
      _isTextureDisablingAllowed = isAllowed; // only new GPU; not affected by reset
    }
    void debugTexturesLegacy(uint32_t params) { // only old GPU; not affected by reset
      switch (params & 0x0FFFu) {
        case 0x501u: _debugDisableTextures = false; break;
        case 0x504u: _debugDisableTextures = true; break;
        default: break;
      }
    }
  
  private:
    ControlStatusRegister _statusRegister;
    uint32_t _dataRequestResult{ 0x400u };
    bool _isTextureDisablingAllowed{ true };
    bool _debugDisableTextures{ false };
    // FIFO buffer 64b (3 commands max): 
      // à chaque commande qui doit aller en FIFO, faire ++fifoSize au début de l'appel, et (if fifoSize > 0) --fifoSize quand le traitement est fini.
      // si fifoSize >= 3, setter bit pour signaler fifo full
      // si clear fifo: annuler les commandes en cours ; fifoSize = 0
      
      // -> utiliser pattern::Contract avec objet FIFO: en guise de ++, ajout du contrat à une liste, et comme --, le retrait après revoke
      //    + permettre à chaque commande de vérifier si contrat toujours valide (avant de poster leur résultat dans framebuffer) -> valeur l'indique (et mise à jour par objet fifo en cas de clear) -> revoké ensuite par la commande qui abandonne
      //    + permettre, depuis l'objet FIFO, d'attendre tous les revoke, avant de passer à la suite (en cas de clear)
      //    MAIS l'objet fifo devra être locké pour empêcher que de nouveaux contrats soient créés tant que le clear n'est pas fini !
      // -> si nouvelle commande remplit la fifo, un bool le lui signale -> set statut fifo full
      // -> si fifo déjà pleine, elle refuse la création de contrat et la commande est abandonnée ?
      // -> si fifo était pleine et qu'une commande se termine (ou clear) -> set statut fifo ready
      
    // texture cache 2kB -> partagé et injecté via constructeur ?
    // frame buffer 2MB -> partagé et injecté via constructeur ?
      // frame buffer agrandi (x2/x4/x8) serait-il totalement côté opengl, textures comprises ?
  };
}
