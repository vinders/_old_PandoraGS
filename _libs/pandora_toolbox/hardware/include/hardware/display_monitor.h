/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace pandora { 
  namespace hardware {
    /// @brief Display position/area ("virtual desktop" coordinates)
    struct DisplayArea final {
      int32_t x;       ///< Horizontal position of a screen/context/window. Can be negative if it's not the primary display.
      int32_t y;       ///< Vertical position of a screen/context/window. Can be negative if it's not the primary display.
      uint32_t width;  ///< Horizontal size of a screen/context/window.
      uint32_t height; ///< Vertical size of a screen/context/window.
    };

    // ---

    /// @class DisplayMonitor
    /// @brief Display monitor description (handle and attributes) + discovery utilities.
    class DisplayMonitor final {
    public:
#     ifdef _WINDOWS
        using Id = std::wstring;
        using Handle = void*;
        using WindowHandle = void*;
#     else
        using Id = int32_t;
        using Handle = int32_t;
        using WindowHandle = int32_t;
#     endif

      /// @brief Attributes of a display monitor on a system
      struct Attributes final {
        Id id;                  ///< Unique identifier on the system.
        DisplayArea screenArea; ///< Display position/area of the entire screen in "virtual desktop". Can contain negative values if not a primary monitor.
        DisplayArea workArea;   ///< Max work area in "virtual desktop": screen area excluding taskbar/toolbars. Can contain negative values if not a primary monitor.
        bool isPrimary;         ///< Primary or secondary display monitor.
      };

      DisplayMonitor() = default;
      DisplayMonitor(Handle monitorHandle, bool& isValid) noexcept;
      DisplayMonitor(const DisplayMonitor&) = default;
      DisplayMonitor(DisplayMonitor&&) = default;
      DisplayMonitor& operator=(const DisplayMonitor&) = default;
      DisplayMonitor& operator=(DisplayMonitor&&) = default;
      ~DisplayMonitor() = default;

      // -- factory --

      /// @brief Create primary monitor description
      /// @returns Success (valid handle and attributes) or not (empty data)
      static bool createPrimaryMonitor(DisplayMonitor& out) noexcept;
      /// @brief Create monitor description from its identifier
      /// @returns Success (valid handle and attributes) or not (empty data)
      static bool createMonitorById(DisplayMonitor::Id id, DisplayMonitor& out) noexcept;
      /// @brief Create monitor description from its index (0 based)
      /// @returns Success (valid handle and attributes) or not (empty data)
      static bool createMonitorByIndex(uint32_t index, DisplayMonitor& out) noexcept;

      /// @brief Get list of monitors available for display
      static std::vector<DisplayMonitor> listAvailableMonitors() noexcept;

      // -- accessors --

      /// @brief Get native monitor handle
      inline Handle handle() noexcept { return this->_handle; }
      /// @brief Get display attributes of monitor
      inline const Attributes& attributes() const noexcept { return this->_attributes; }
      
      /// @brief Read per-monitor DPI (if Win10+) or system DPI value
      /// @warning The process must be DPI aware (on Windows: requires manifest or calling enableDpiAwareness)
      uint32_t getMonitorDpi(WindowHandle windowHandle) const noexcept;
      /// @brief Read monitor content scale (based on system DPI)
      /// @warning The process must be DPI aware (on Windows: requires manifest or calling enableDpiAwareness)
      void getMonitorContentScale(float& factorX, float& factorY) const noexcept;

      // -- utilities --

      /// @brief Enable DPI awareness for current process
      /// @warning - on Windows, it is recommended to use a manifest instead of this call.
      ///          - this function should be called once at the beginning of the process to avoid weird behaviors.
      static bool enableDpiAwareness() noexcept;

      /// @brief Get primary monitor handle
      static DisplayMonitor::Handle getPrimaryMonitorHandle() noexcept;


      //TODO actions:
      // changer résolution d'affichage fullscreen
      // restaurer celle par défaut
      // créer monitor ??? / adapter ???
      // adjust window rect <-> client area (+ version manuelle calcul -> metrics taille bordures+caption+scrollbars+menu)

      //TODO metrics: 
      // resolutionXY -> EnumDisplaySettingsExW(nullptr, ENUM_CURRENT_SETTINGS, &deviceInfo, 0) et deviceInfo.dmPelsWidth /OU/ GetDeviceCaps(screenDC, HORZRES) et VERTRES /OU/ GetSystemMetricsForDpi(SM_CXSCREEN, dpi) /OU/ GetSystemMetrics(SM_CXSCREEN)
      // work area manuel -> SystemParametersInfoA(SPI_GETWORKAREA, 0, &rect, 0)
      // taille en millimètres -> /Win8.1/ GetDeviceCaps(dc, HORZSIZE); et VERTSIZE /OU/ dm.dmPelsWidth * 25.4f / GetDeviceCaps(dc, LOGPIXELSX));

    private:
      Handle _handle = (Handle)0;
      Attributes _attributes{ Id{}, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, false };
    };
  }
}
