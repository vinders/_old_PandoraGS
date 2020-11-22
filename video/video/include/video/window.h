/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window management for desktop systems (Windows / Mac OS / Linux / BSD)
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <mutex>
#ifdef _WINDOWS
# include <system/api/windows_api.h>
#endif
#include <thread/spin_lock.h>
#include <system/preprocessor_tools.h>
#include "./window_style.h"

namespace pandora {
  namespace video {
#   ifdef _WINDOWS
      using WindowHandle   = HWND;    ///< Window management handle
#   else
      using WindowHandle   = int32_t; ///< Window management handle
#   endif

    /// @brief Window size features bits (fixed/resizable)
    /// @warning Ignored with WindowDisplayMode::fullscreen/WindowDisplayMode::borderless.
    enum class WindowSizeMode : uint32_t {
      fixed       = 0u,   ///< fixed width, fixed height
      resizableX  = 0x1u, ///< resizable width (if 'resizableY' is also set, allows maximization)
      resizableY  = 0x2u, ///< resizable height (if 'resizableY' is also set, allows maximization)
      homothety   = 0x4u  ///< homothety (ratio kept when resized).
    };
    /// @brief Window widget features bits
    /// @warning Ignored with WindowDisplayMode::fullscreen/WindowDisplayMode::borderless.
    enum class WindowWidgetMode : uint32_t {
      none         = 0u,    ///< no additional feature
      scrollbarX   = 0x01u, ///< horizontal scroll bar
      scrollbarY   = 0x02u, ///< vertical scroll bar
      topMost      = 0x04u, ///< window displayed above all non-topmost windows
      aboveTaskbar = 0x08u  ///< window displayed above taskbar
    };
    
    /// @brief Window visibility change
    enum class WindowVisibility : uint32_t {
#     ifdef _WINDOWS
        hide         = SW_HIDE,         ///< Hide window, and activate the next top-level window (not recommended on creation).
        show         = SW_SHOW,         ///< Show window with current size/position (active).
        showInactive = SW_SHOWNA,       ///< Show window with current size/position (inactive).
        showDefault  = SW_SHOWDEFAULT,  ///< Show default window state (defined during the process creation).
        restore      = SW_RESTORE,      ///< Show window with original size/position (active, not minimized, nor maximized). 
                                        ///  If a Window is created in hidden mode, 'restore' won't work until a 'show[...]' or 'maximize' visiblity is set.
        minimize     = SW_MINIMIZE,     ///< Minimize window, and activate the next top-level window (not recommended on creation).
        maximize     = SW_SHOWMAXIMIZED,///< Maximize window (active).
#     else
        hide         = 0u, ///< Hide window, and activate the next top-level window.
        show         = 1u, ///< Show window with current size/position (active).
        showInactive = 2u, ///< Show window with current size/position (inactive).
        showDefault  = 3u, ///< Show default window state (defined during the process creation).
        restore      = 4u, ///< Show window with original size/position (active, not minimized, nor maximized).
        minimize     = 5u, ///< Minimize window, and activate the next top-level window.
        maximize     = 6u, ///< Maximize window (active).
#     endif
    };
    
    constexpr inline uint32_t defaultWindowSize() noexcept { return 0u; }               ///< width or height to set for default window size
    constexpr inline uint32_t defaultWindowPosition() noexcept { return 0xFFFFFFFFu; }  ///< X or Y coordinate for default window position
    constexpr inline uint32_t centeredWindowPosition() noexcept { return 0xFFFFFFFEu; } ///< X or Y coordinate for centered window position
    
    /// @brief Pixel dimensions of a window
    struct WindowPosition final {
      uint32_t clientAreaWidth = defaultWindowSize();
      uint32_t clientAreaHeight = defaultWindowSize();
      uint32_t x = defaultWindowPosition();
      uint32_t y = defaultWindowPosition();
    };
    /// @brief Pixel X/Y coordinates or size of a window
    struct WindowPixel final {
      int32_t x;
      int32_t y;
    };
    
    // ---

    /// @class Window
    /// @brief System window object (to display content, menus, controls, video rendering...)
    /// @remarks Example of creation: 
    ///            auto styleDef = WindowStyleBuild(hInstance).setDisplayMode(WindowDisplayMode::fullscreen)
    ///                                                       .build(_P_WINDOW_STRING("FULLSCR_WINDOW_TEMPLATE"));
    ///            Window myWindow(WindowPosition{800, 600}, styleDef, WindowWidgetMode::topMost, WindowSizeMode::fixed, 
    ///                            _P_WINDOW_STRING("My Window"), false, WindowVisibility::show);
    class Window final {
    public:
      /// @brief Create a new system window
      /// @warning Throws on creation failure.
#     ifdef _WINDOWS
        Window(const WindowPosition& position, std::shared_ptr<WindowStyle> windowStyleDef, 
               WindowWidgetMode widgets, WindowSizeMode sizeMode, const wchar_t caption[], bool isCursorVisible, 
               WindowVisibility initialStatus, std::shared_ptr<MenuResource> menu = nullptr, WindowHandle owner = (WindowHandle)0);
#     else
        Window(const WindowPosition& position, std::shared_ptr<WindowStyle> windowStyleDef, 
               WindowWidgetMode widgets, WindowSizeMode sizeMode, const char caption[], bool isCursorVisible, 
               WindowVisibility initialStatus, std::shared_ptr<MenuResource> menu = nullptr, WindowHandle owner = (WindowHandle)0);
#     endif
      ~Window() noexcept;

      // no move: once registered to event handler, the window address must not change
      Window(const Window&) = delete;
      Window(Window&&) = delete; 
      Window& operator=(const Window&) = delete;
      Window& operator=(Window&&) = delete;
      
      // -- accessors --
      
      /// @brief Get native window handle
      inline WindowHandle WindowDisplayMode handle() noexcept { return this->_windowHandle; }
      
      /// @brief Get current window display mode (fullscreen/borderless/window/...)
      inline WindowDisplayMode displayMode() const noexcept { return this->_displayMode; }
      /// @brief Get current widget mode
      inline WindowWidgetMode widgets() const noexcept { return this->_widgets; }
      /// @brief Get current resizing mode (resizable X/Y, homothety)
      inline WindowSizeMode sizeMode() const noexcept { return this->_sizeMode; }
      
      /// @brief Get current size of the client area of the window
      WindowPixel clientAreaSize() const noexcept;
      /// @brief Get current screen position of the cursor (may be out of the window)
      /// @warning Returns { -1, -1 } if the cursor is not available
      WindowPixel cursorScreenPosition() noexcept;
      /// @brief Get current position of the cursor within the window client area
      /// @warning Returns { -1, -1 } if the cursor is not available, hidden or out of the window
      WindowPixel cursorWindowPosition() noexcept;
      
      // -- change display settings --
      
      /// @brief Change window display mode (fullscreen/borderless/window/...)
      bool displayMode(WindowDisplayMode mode) noexcept;
      /// @brief Change window display mode + resizing mode
      bool displayMode(WindowDisplayMode mode, WindowSizeMode sizeMode) noexcept;
      /// @brief Change window resizing mode
      bool sizeMode(WindowSizeMode sizeMode) noexcept;
      /// @brief Change window visibility/focus or maximize/minimize window
      bool visibility(WindowVisibility status) noexcept;
      
      /// @brief Change window cursor shape/resource
      bool cursorType(std::shared_ptr<CursorResource> cursor) noexcept;
      /// @brief Set cursor position in screen coordinates
      bool cursorWindowPosition(uint32_t absoluteX, uint32_t absoluteY) noexcept;
      /// @brief Set cursor position in window coordinates (client area)
      bool cursorScreenPosition(uint32_t clientAreaX, uint32_t clientAreaY) noexcept;
      /// @brief Show/hide cursor
      void cursorVisibility(bool isVisible) noexcept;

      /// @brief Change main title/caption of window
#     ifdef _WINDOWS
        bool caption(const wchar_t caption[]) noexcept;
#     else
        bool caption(const char caption[]) noexcept;
#     endif
      /// @brief Change main menu bar of window
      bool menuType(std::shared_ptr<MenuResource> menu) noexcept;
      
      // -- draw rectangle --

      /// @brief Replace entire client area with background "empty" color
      void clearClientArea() noexcept;
      /// @brief Replace a specific zone of the client area with background "empty" color
      void clearClientArea(const WindowPosition& position) noexcept;
      /// @brief Replace entire client area with any color
      void fillClientArea(std::shared_ptr<ColorResource> color) noexcept;
      /// @brief Replace a specific zone of the client area with any color
      void fillClientArea(const WindowPosition& position, std::shared_ptr<ColorResource> color) noexcept;

      // -- change size / position --

      /// @brief Resize window without changing its position
      bool resize(uint32_t width, uint32_t height) noexcept;
      /// @brief Move window without changing its size
      bool move(uint32_t x, uint32_t y) noexcept;
      /// @brief Change window size and position
      bool moveResize(const WindowPosition& position) noexcept;
      

    private:
#     ifdef _WINDOWS
        void _refreshClientSizeX() noexcept;
        void _refreshClientSizeY() noexcept;

        inline RECT _getClientArea() noexcept { 
          std::lock_guard<pandora::thread::SpinLock> lock(_clientAreaLock);
          RECT area = this->_clientArea;
          return area;
        }
        inline RECT _getClientArea(double& ratio) noexcept { 
          std::lock_guard<pandora::thread::SpinLock> lock(_clientAreaLock);
          ratio = this->_sizeRatio;
          RECT area = this->_clientArea;
          return area;
        }
        inline double _getClientAreaRatio() noexcept { 
          std::lock_guard<pandora::thread::SpinLock> lock(_clientAreaLock);
          return this->_sizeRatio;
        }
        inline void _setClientArea(RECT area) noexcept { 
          std::lock_guard<pandora::thread::SpinLock> lock(_clientAreaLock);
          this->_clientArea = area;
          if ((this->_sizeMode & WindowSizeMode::homothety) && area.bottom > area.top)
            this->_sizeRatio = static_cast<double>(area.right - area.left) / static_cast<double>(area.bottom - area.top);
        }
        inline void _setClientAreaForceRatio(RECT area) noexcept { 
          std::lock_guard<pandora::thread::SpinLock> lock(_clientAreaLock);
          this->_clientArea = area;
          this->_sizeRatio = static_cast<double>(area.right - area.left) / static_cast<double>(area.bottom - area.top);
        }
#     endif
    
    private:
      // keep resources and definition alive while window exists
      std::shared_ptr<WindowStyle> _windowStyleDef = nullptr;
      std::shared_ptr<MenuResource> _menu = nullptr;
      std::shared_ptr<ColorResource> _backgroundColor = nullptr;
      std::shared_ptr<CursorResource> _cursor = nullptr;
#     ifdef _WINDOWS
        RECT _clientArea{ 0 };
#     endif
      double _sizeRatio = 1.0;
      pandora::thread::SpinLock _clientAreaLock;
      
      WindowHandle _windowHandle = nullptr;
      WindowDisplayMode _displayMode = WindowDisplayMode::window;
      WindowWidgetMode _widgets = WindowWidgetMode::none;
      WindowSizeMode _sizeMode = WindowSizeMode::fixed;
      bool _isCursorVisible = true;
    };
  }
}
_P_FLAGS_OPERATORS(pandora::video::WindowSizeMode, uint32_t)
_P_FLAGS_OPERATORS(pandora::video::WindowWidgetMode, uint32_t)
