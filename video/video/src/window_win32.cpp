/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window management: Win32 implementation (Windows)
*******************************************************************************/
#ifdef _WINDOWS
# include <cstdint>
# include <cstddef>
# include <cstring>
# include <system/api/windows_api.h>
# include <windowsx.h>
# include "video/window_style.h"
# include "video/window.h"

# if !defined(_CPP_REVISION) || _CPP_REVISION != 14
#   define __if_constexpr if constexpr
# else
#   define __if_constexpr if
# endif

using namespace pandora::video;

// -- display/size/style native enums -- ---------------------------------------

  // convert window display mode enum to native flags
  template <bool _ResetPrevious>
  static inline void _setWindowDisplayFlags(WindowDisplayMode displayMode, DWORD& outStyle, DWORD& outExtStyle) noexcept {
    __if_constexpr (_ResetPrevious) {
      outStyle    &= ~((DWORD)(WS_OVERLAPPED | WS_POPUP | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX));
      outExtStyle &= ~((DWORD)WS_EX_TOOLWINDOW);
    }
    outStyle |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    
    switch (params.displayMode) {
      case WindowDisplayMode::fullscreen: outStyle |= WS_POPUP; 
                                          break;
      case WindowDisplayMode::borderless: outStyle |= WS_POPUP; 
                                          break;
      case WindowDisplayMode::dialog:     outStyle |= (WS_POPUP | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU); 
                                          break;
      case WindowDisplayMode::window:     outStyle |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX); 
                                          break;
      case WindowDisplayMode::floatingWindow: outStyle |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
                                              outExtStyle |= WS_EX_TOOLWINDOW;
                                              break;
      default: outStyle |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX); break;
    }
  }

  // convert window size/widget enums to native flags
  template <bool _ResetPrevious>
  static inline void _setWindowWidgetFlags(WindowSizeMode sizeMode, WindowWidgetMode widgets, bool isResizableDisplay, DWORD& outStyle, DWORD& outExtStyle) noexcept {
    __if_constexpr (_ResetPrevious) {
      outStyle    &= ~((DWORD)(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_HSCROLL | WS_VSCROLL));
      outExtStyle &= ~((DWORD)(WS_EX_TOPMOST | WS_EX_APPWINDOW | WS_EX_ACCEPTFILES));
    }
    if (isResizableDisplay && (sizeMode & (WindowSizeMode::resizableX | WindowSizeMode::resizableY))) { // no size style in fullscreen mode
      outStyle |= WS_SIZEBOX;
      if ((sizeMode & (WindowSizeMode::resizableX | WindowSizeMode::resizableY)) == (WindowSizeMode::resizableX | WindowSizeMode::resizableY))
        outStyle |= WS_MAXIMIZEBOX;
    }
    
    if (widgets & WindowWidgetMode::scrollbarX)
      outStyle |= WS_HSCROLL;
    if (widgets & WindowWidgetMode::scrollbarY)
      outStyle |= WS_VSCROLL;
    if (widgets & WindowWidgetMode::topMost)
      outExtStyle |= WS_EX_TOPMOST;
    if (widgets & WindowWidgetMode::aboveTaskbar)
      outExtStyle |= WS_EX_APPWINDOW;
  }


// -- native resource management -- --------------------------------------------

  // -- icon resource management --

  std::shared_ptr<IconResource> WindowStyleBuilder::createSystemIcon(SystemIcon id) noexcept {
    ResourceHandle handle = (ResourceHandle)LoadIconW(nullptr, id);
    return (handle != nullptr) ? std::make_shared<IconResource>(handle) : nullptr;
  }

  std::shared_ptr<IconResource> WindowStyleBuilder::createIconFromFile(const char uri[], uint32_t width, uint32_t height) noexcept {
    UINT flags = LR_LOADFROMFILE;
    if (width == 0u || height == 0u)
      flags |= LR_DEFAULTSIZE;
    ResourceHandle handle = (ResourceHandle)LoadImageA(nullptr, uri, IMAGE_ICON, width, height, flags)
    return (handle != nullptr) ? std::make_shared<IconResource>(handle) : nullptr;
  }
  std::shared_ptr<IconResource> WindowStyleBuilder::createIconFromFile(const wchar_t uri[], uint32_t width, uint32_t height) noexcept {
    UINT flags = LR_LOADFROMFILE;
    if (width == 0u || height == 0u)
      flags |= LR_DEFAULTSIZE;
    ResourceHandle handle = (ResourceHandle)LoadImageW(nullptr, uri, IMAGE_ICON, width, height, flags)
    return (handle != nullptr) ? std::make_shared<IconResource>(handle) : nullptr;
  }

  std::shared_ptr<IconResource> WindowStyleBuilder::readInternalIcon(const char id[]) const noexcept {
    ResourceHandle handle = (ResourceHandle)LoadIconA(this->_resource.appInstance, id)
    return (handle != nullptr) ? std::make_shared<IconResource>(handle) : nullptr;
  }
  std::shared_ptr<IconResource> WindowStyleBuilder::readInternalIcon(const wchar_t id[]) const noexcept {
    ResourceHandle handle = (ResourceHandle)LoadIconW(this->_resource.appInstance, id)
    return (handle != nullptr) ? std::make_shared<IconResource>(handle) : nullptr;
  }

  IconResource::~IconResource() noexcept { 
    if (this->_handle != nullptr)
      DestroyIcon((HICON)this->_handle); 
  }

  // -- cursor resource management --

  std::shared_ptr<CursorResource> WindowStyleBuilder::createSystemCursor(SystemCursor id) noexcept {
    ResourceHandle handle = (ResourceHandle)LoadCursorW(nullptr, id)
    return (handle != nullptr) ? std::make_shared<CursorResource>(handle) : nullptr;
  }

  std::shared_ptr<CursorResource> WindowStyleBuilder::createCursorFromFile(const char uri[]) noexcept {
    ResourceHandle handle = (ResourceHandle)LoadImageA(nullptr, uri, IMAGE_CURSOR, width, height, LR_LOADFROMFILE)
    return (handle != nullptr) ? std::make_shared<CursorResource>(handle) : nullptr;
  }
  std::shared_ptr<CursorResource> WindowStyleBuilder::createCursorFromFile(const wchar_t uri[]) noexcept {
    ResourceHandle handle = (ResourceHandle)LoadImageW(nullptr, uri, IMAGE_CURSOR, width, height, LR_LOADFROMFILE)
    return (handle != nullptr) ? std::make_shared<CursorResource>(handle) : nullptr;
  }

  std::shared_ptr<CursorResource> WindowStyleBuilder::readInternalCursor(const char id[]) const noexcept {
    ResourceHandle handle = (ResourceHandle)LoadCursorA(this->_resource.appInstance, id)
    return (handle != nullptr) ? std::make_shared<CursorResource>(handle) : nullptr;
  }
  std::shared_ptr<CursorResource> WindowStyleBuilder::readInternalCursor(const wchar_t id[]) const noexcept {
    ResourceHandle handle = (ResourceHandle)LoadCursorW(this->_resource.appInstance, id)
    return (handle != nullptr) ? std::make_shared<CursorResource>(handle) : nullptr;
  }

  CursorResource::~CursorResource() noexcept { 
    if (this->_handle != nullptr)
      DestroyCursor((HCURSOR)this->_handle); 
  }

  // -- color resource management --

  std::shared_ptr<ColorResource> WindowStyleBuilder::createColorStyle(WindowColor color) noexcept {
    ResourceHandle handle = nullptr;
    switch (color) {
      case WindowStyleBuilder::systemWindowColor(): handle = (ResourceHandle)GetSysColorBrush(COLOR_WINDOW+1); break;
      case WindowStyleBuilder::systemTextColor():   handle = (ResourceHandle)GetSysColorBrush(COLOR_WINDOWTEXT+1); break;
      case 0:           handle = (ResourceHandle)GetStockObject(BLACK_BRUSH); break;
      case 0x00FFFFFFu: handle = (ResourceHandle)GetStockObject(WHITE_BRUSH); break;
      default:          handle = (ResourceHandle)CreateSolidBrush((COLORREF)(color & 0x00FFFFFFu)); break;
    }
    return (handle != nullptr) ? std::make_shared<ColorResource>(handle) : nullptr;
  }
  
  ColorResource::~ColorResource() noexcept { 
    if (this->_handle != nullptr)
      DeleteObject((HBRUSH)this->_handle); 
  }


// -- window event processing -- -----------------------------------------------

  static LRESULT CALLBACK _windowEventHandler(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam) {
    //TODO
    //...
    //...
    //...
    //...
    
    // si RESIZE ou MAXIMIZE ou RESTORE ou déminimisé
      // màj _clientArea + _sizeRatio (sauf homothety)
      // si en mode 'homothety', 
        //ne pas mettre à jour _sizeRatio
        //adapter la hauteur ou largeur affichée: selon le plus proche de ce que fait le user, et selon taille écran (ne pas déborder)
      // si fin de redimensionnement (pas de drag en cours) et pas fullscreen/borderless:
        // adapter largeur/hauteur: multiples de 8 pixels (ex: 639->640) SAUF si dimension est 1366 (les autres sont toutes divisibles par 8)
        
      // en cas de resize, appeler callback onResize de l'objet Window (si non nul)
      // en cas de WM_PAINT, appeler callback onPaint de l'objet Window (si non nul)
    return DefWindowProc(windowHandle, message, wparam, lparam);
  }


// -- window definition class creation/destruction -- --------------------------

  WindowStyleBuilder::WindowStyleBuilder(InstanceHandle appInstance) noexcept {
    _resources.appInstance = (appInstance != nullptr) ? appInstance : (InstanceHandle)GetModuleHandleW(nullptr);
  }

  // create style definition class
  std::shared_ptr<WindowStyle> WindowStyleBuilder::build(const wchar_t uniqueStyleName[]) noexcept {
    if (uniqueStyleName == nullptr || *uniqueStyleName == static_cast<wchar_t>(0))
      return nullptr;
    
    WNDCLASSEXW windowClass;
    ZeroMemory(&windowClass, sizeof(WNDCLASSEXW));
    
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.lpszClassName = uniqueStyleName;
    windowClass.lpszMenuName = nullptr;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = (this->_resources.displayMode == WindowDisplayMode::dialog) ? 0 : DLGWINDOWEXTRA;
    windowClass.lpfnWndProc = _windowEventHandler;
    windowClass.hInstance = this->_resources.appInstance;
    
    windowClass.style = ((UINT)(this->_tweaks) | CS_HREDRAW | CS_VREDRAW);
    if (this->_resources.backgroundColor == nullptr)
      this->_resources.backgroundColor = std::make_shared<ColorResource>((ResourceHandle)GetSysColorBrush(COLOR_WINDOW+1));
    windowClass.hbrBackground = (HBRUSH)(this->_resources.backgroundColor->handle());
    
    if (this->_resources.cursor == nullptr)
      this->_resources.cursor = std::make_shared<CursorResource>((ResourceHandle)LoadCursorW(nullptr, IDC_ARROW));
    windowClass.hCursor = (HCURSOR)(this->_resources.cursor->handle());
    if (this->_resources.appIcon != nullptr)
      windowClass.hIcon = (HICON)(this->_resources.appIcon->handle());
    if (this->_resources.captionIcon != nullptr)
      windowClass.hIconSm = (HICON)(this->_resources.captionIcon->handle()); 

    return (RegisterClassExW(&windowClass, (HINSTANCE)(this->_resources.appInstance)) != 0)
            ? std::make_shared<WindowStyle>(uniqueStyleName, this->_resources) 
            : nullptr;
  }
  
  // destroy style definition class
  WindowStyle::~WindowStyle() noexcept {
    if (!this->_styleName.empty())
      UnregisterClassW(this->_styleName.c_str(), (HINSTANCE)(this->_resources.appInstance));
  }


// -- window metrics -- --------------------------------------------------------

  static inline void _getWindowBorderSizes(WindowDisplayMode displayMode, WindowSizeMode sizeMode, uint32_t& outBorderX, uint32_t& outBorderY, uint32_t& outCaptionY) noexcept {
    if (displayMode == WindowDisplayMode::fullscreen || displayMode == WindowDisplayMode::borderless) {
      outBorderX = 0;
      outBorderY = 0;
      outCaptionY = 0;
    }
    else {
      if (sizeMode & (WindowSizeMode::resizableX | WindowSizeMode::resizableY)) {
        outBorderX = GetSystemMetrics(SM_CXSIZEFRAME);
        outBorderY = GetSystemMetrics(SM_CYSIZEFRAME);
      }
      else {
        outBorderX = GetSystemMetrics(SM_CXFIXEDFRAME);
        outBorderY = GetSystemMetrics(SM_CYFIXEDFRAME);
      }
      outCaptionY = GetSystemMetrics(SM_CYCAPTION);
    }
  }
  
  static inline bool _readClientArea(HWND windowHandle, RECT& out) noexcept {
    return (GetClientRect(windowHandle, &out) != FALSE && out.right > out.left && out.bottom > out.top);
  }
  static inline void _calculateClientArea(const WindowPosition& position, WindowDisplayMode displayMode, WindowSizeMode sizeMode, RECT& out) noexcept {
    uint32_t borderX, borderY, captionY;
    _getWindowBorderSizes(displayMode, sizeMode, borderX, borderY, captionY);
    
    out.left = static_cast<LONG>(position.x + borderX);
    out.top = static_cast<LONG>(position.y + captionY);
    out.right = out.left + static_cast<LONG>(position.clientAreaWidth);
    out.bottom = out.top + static_cast<LONG>(position.clientAreaHeight);
  }
  static inline void _calculateTotalArea(const WindowPosition& position, WindowDisplayMode displayMode, WindowSizeMode sizeMode, RECT& out) noexcept {
    uint32_t borderX, borderY, captionY;
    _getWindowBorderSizes(displayMode, sizeMode, borderX, borderY, captionY);
    
    out.left = position.x;
    out.top = position.y;
    out.right = out.left + position.clientAreaWidth + (borderX<<1);
    out.bottom = out.top + position.clientAreaHeight + captionY + borderY;
  }
  
  void Window::_refreshClientSizeX() noexcept {
    RECT area;
    if (_readClientArea(area)) {
      _setClientArea(area);
      
      if ((this->_sizeMode & WindowSizeMode::homothety)
      && this->_displayMode != WindowDisplayMode::fullscreen && this->_displayMode != WindowDisplayMode::borderless) {
        LONG newSize = static_cast<LONG>(static_cast<double>(area.bottom - area.top)*_getClientAreaRatio());
        if (newSize != area.right - area.left)
          SetWindowPos(this->_windowHandle, HWND_TOP, 0, 0, newSize, area.bottom - area.top, SWP_NOMOVE);
      }
    }
  }
  void Window::_refreshClientSizeY() noexcept {
    RECT area;
    if (_readClientArea(area)) {
      _setClientArea(area);
      
      if ((this->_sizeMode & WindowSizeMode::homothety)
      && this->_displayMode != WindowDisplayMode::fullscreen && this->_displayMode != WindowDisplayMode::borderless) {
        LONG newSize = static_cast<LONG>(static_cast<double>(area.right - area.left)/_getClientAreaRatio());
        if (newSize != area.bottom - area.top)
          SetWindowPos(this->_windowHandle, HWND_TOP, 0, 0, area.right - area.left, newSize, SWP_NOMOVE);
      }
    }
  }
  

// -- window creation/destruction -- -------------------------------------------

  template <bool _IsFirstCall>
  static bool _setWindowVisibility(WindowHandle window, WindowDisplayMode displayMode, WindowVisibility status) noexcept {
    __if_constexpr (_IsFirstCall) {
      if (initialStatus == WindowVisibility::hide)
        return true;
    }
    
    int visibility = (int)status;
    if (this->_displayMode == WindowDisplayMode::fullscreen) {
      if (status != WindowVisibility::minimize)
        visibility = SW_SHOWMAXIMIZED;
    }
    else __if_constexpr (_IsFirstCall) {
      if (status == WindowVisibility::restore)
        visibility = SW_SHOWNORMAL;
    }
    return (ShowWindow(window, visibility) != FALSE);
  }

  // create window
  Window::Window(const WindowPosition& position, std::shared_ptr<WindowStyle> windowStyleDef, WindowWidgetMode widgets, WindowSizeMode sizeMode,
                 const wchar_t caption[], bool isCursorVisible, WindowVisibility initialStatus, std::shared_ptr<MenuResource> menu, WindowHandle owner)
    : _windowStyleDef(std::move(windowStyleDef)),
      _menu(std::move(menu)),
      _widgets(widgets),
      _sizeMode(sizeMode),
      _isCursorVisible(isCursorVisible) {
    this->_backgroundColor = this->_windowStyleDef.backgroundColor;
    this->_displayMode = this->_windowStyleDef->displayMode;
    assert(this->_backgroundColor != nullptr);

    // set window style
    DWORD styleFlags = 0;
    DWORD styleExtFlags = 0;
    bool isResizable = (this->_displayMode == WindowDisplayMode::fullscreen || this->_displayMode == WindowDisplayMode::borderless);
    _setWindowDisplayFlags<false>(this->_displayMode, styleFlags, styleExtFlags);
    _setWindowWidgetFlags<false>(sizeMode, widgets, isResizable, styleFlags, styleExtFlags);
    
    // set window position
    //if (position.clientAreaWidth == 0)
      //CW_USEDEFAULT ou taille SM_CXFULLSCREEN en fullscreen ?
    //if (position.clientAreaHeight == 0)
      //CW_USEDEFAULT ou taille SM_CYFULLSCREEN en fullscreen ?
    //si pas CW_USEDEFAULT, 
      //if (AdjustWindowRectExW == FALSE)
        //_calculateTotalArea(position, this->_displayMode, sizeMode, this->_clientArea);
    //...
    //TODO: détection adapter/monitor (nb écrans, principal, taille limite...)
    
    //TODO: set DPI + pixel-format + CreateWindowExW (caption, menu, owner)
    // ChangeDisplaySettingsExW si fullscreen ???
    //NB: ajouter création menu dans WindowStyleBuilder
    //...
    if (this->_windowHandle == nullptr)
      throw std::invalid_argument("Window: window creation failed."); //TODO: GetLastError + FormatMessage

    // display window
    if (!_setWindowVisibility<true>(this->_windowHandle, this->_displayMode, initialStatus))
      throw std::invalid_argument("Window: failed to display window with the requested visibility mode."); //TODO: GetLastError + FormatMessage
    if (!isCursorVisible)
      ShowCursor(false);
    
    // store client area
    if (initialStatus != WindowVisibility::hide && initialStatus != WindowVisibility::minimize) {
      if (!_readClientArea(this->_windowHandle, this->_clientArea))
        _calculateClientArea(position, this->_displayMode, sizeMode, this->_clientArea);
    }
    else
      _calculateClientArea(position, this->_displayMode, sizeMode, this->_clientArea);

    // client area ratio: used for homothety
    if ((sizeMode & WindowSizeMode::homothety) || this->_clientArea.bottom == this->_clientArea.top) {
      if (position.clientAreaWidth == 0)
        position.clientAreaWidth = this->_clientArea.right - this->_clientArea.left;
      if (position.clientAreaHeight == 0)
        position.clientAreaHeight = this->_clientArea.bottom - this->_clientArea.top; // if still 0, use 4:3
      this->_sizeRatio = (position.clientAreaHeight > 0) ? static_cast<double>(position.clientAreaWidth) / static_cast<double>(position.clientAreaHeight) : 4.0/3.0;
    }
    else
      this->_sizeRatio = static_cast<double>(this->_clientArea.right - this->_clientArea.left) / static_cast<double>(this->_clientArea.bottom - this->_clientArea.top);
        
    // provide event handler with address of current object as "user data"
    SetLastError(0);
    if (SetWindowLongPtr(this->_windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)) == 0 && GetLastError() != 0)
      throw std::runtime_error("Window: failed to set user data for window message handling."); //TODO: GetLastError + FormatMessage
    
    // enable window thread + user input
    if (initialStatus != WindowVisibility::minimize) {
      SetForegroundWindow(this->_windowHandle);
      if (initialStatus != WindowVisibility::showInactive)
        SetFocus(this->_windowHandle);
    }
  }
  
  // destroy window
  Window::~Window() noexcept {
    if (this->_windowHandle != nullptr) {
      //if (this->_displayMode == WindowDisplayMode::fullscreen)
        //... ChangeDisplaySettingsExW ??? -> fenêtre + restaurer valeurs
      if (this->_isCursorVisible == false)
        ShowCursor(true);
      
      DestroyWindow(this->_windowHandle);
      this->_windowHandle = nullptr;
    }
  }
  
  // -- accessors --
  
  WindowPixel Window::clientAreaSize() const noexcept {
    std::lock_guard<pandora::thread::SpinLock> lock(_clientAreaLock);
    return { this->_clientArea.right - this->_clientArea.left, this->_clientArea.bottom - this->_clientArea.top };
  }
  WindowPixel Window::cursorScreenPosition() noexcept {
    POINT pos;
    if (GetCursorPos(&pos) != FALSE)
      return { pos.x, pos.y };
    return { -1, -1 };
  }
  WindowPixel Window::cursorWindowPosition() noexcept {
    POINT pos;
    if (GetCursorPos(&pos) != FALSE) {
      RECT clientArea = _getClientArea();
      if (pos.x >= clientArea.left && pos.x < clientArea.right && pos.y >= clientArea.top && pos.y < clientArea.bottom)
        return { pos.x - clientArea.left, pos.y - clientArea.top };
    }
    return { -1, -1 };
  }

  // -- change display settings - display/size mode --

  bool Window::displayMode(WindowDisplayMode displayMode) noexcept {
    //TODO passage fullscreen/window + DPI/pixel-format
    //...
    //màj _clientArea
    //...
    if (success)
      this->_displayMode = displayMode;
    return success;
  }
  bool Window::displayMode(WindowDisplayMode displayMode, WindowSizeMode sizeMode) noexcept {
    //TODO passage fullscreen/window + DPI/pixel-format
    //...
    //màj _clientArea
    //...
    if (success) {
      this->_displayMode = displayMode;
      this->_sizeMode = sizeMode;
    }
    return success;
  } 
  bool Window::sizeMode(WindowSizeMode sizeMode) noexcept {
    //TODO
  }

  bool Window::visibility(WindowVisibility status) noexcept {
    return _setWindowVisibility<false>(this->_windowHandle, this->_displayMode, status);
  }
  
  // -- change display settings - cursor --
  
  bool Window::cursorType(std::shared_ptr<CursorResource> cursor) noexcept {
    if(...)//TODO
      this->_cursor = cursorResource;
  }
  bool Window::cursorScreenPosition(uint32_t absoluteX, uint32_t absoluteY) noexcept {
    //SetCursorPos
  }
  bool Window::cursorWindowPosition(uint32_t clientAreaX, uint32_t clientAreaY) noexcept {
    //SetCursorPos
  }
  void Window::cursorVisibility(bool isVisible) noexcept {
    if (this->_isCursorVisible == isVisible)
      return true;
    this->_isCursorVisible = isVisible;
    ShowCursor(isVisible);
  }
  
  // -- change display settings - title/menu --

  bool Window::caption(const wchar_t caption[]) noexcept {
    //TODO
  }

  bool Window::menuType(std::shared_ptr<MenuResource> menu) noexcept {
    if (SetMenu(this->_windowHandle, (menu != nullptr && menu->isValid()) ? (HMENU)(menu->handle()) : nullptr) != FALSE) {
      this->_menu = menu; // release old value AFTER it's been replaced in window
      _refreshClientSizeY();
      return true;
    }
    return false;
  }
  
  // -- draw rectangle --

  void Window::clearClientArea() noexcept {
    assert(this->_backgroundColor != nullptr);
    RECT area = _getClientArea();
    FillRect(context, &area, (HBRUSH)(this->_backgroundColor->handle()));
  }
  void Window::clearClientArea(const WindowPosition& position) noexcept {
    assert(this->_backgroundColor != nullptr);
    if (position.width > 0u && position.height > 0u) {
      RECT area;
      area.left = (position.x != defaultWindowPosition()) ? static_cast<LONG>(position.x) : 0;
      area.top = (position.y != defaultWindowPosition()) ? static_cast<LONG>(position.y) : 0;
      area.right = area.left + static_cast<LONG>(position.width);
      area.bottom = area.top + static_cast<LONG>(position.height);
      FillRect(context, &area, (HBRUSH)(this->_backgroundColor->handle()));
    }
  }
  
  void Window::fillClientArea(std::shared_ptr<ColorResource> color) noexcept {
    assert(this->_backgroundColor != nullptr);
    RECT area = _getClientArea();
    FillRect(context, &area, (color != nullptr) ? (HBRUSH)(color->handle()) : (HBRUSH)(this->_backgroundColor->handle()));
  }
  void Window::fillClientArea(const WindowPosition& position, std::shared_ptr<ColorResource> color) noexcept{
    assert(this->_backgroundColor != nullptr);
    if (position.width > 0u && position.height > 0u) {
      RECT area;
      area.left = (position.x != defaultWindowPosition()) ? static_cast<LONG>(position.x) : 0;
      area.top = (position.y != defaultWindowPosition()) ? static_cast<LONG>(position.y) : 0;
      area.right = area.left + static_cast<LONG>(position.width);
      area.bottom = area.top + static_cast<LONG>(position.height);
      FillRect(context, &area, (color != nullptr) ? (HBRUSH)(color->handle()) : (HBRUSH)(this->_backgroundColor->handle()));
    }
  }

  // -- change size / position --

  bool Window::resize(uint32_t width, uint32_t height) noexcept {
    RECT area = _getClientArea();
    //TODO - default -> garder taille fenêtre ou taille SM_CXFULLSCREEN en fullscreen ?
    area.right = area.left + width;
    //TODO - default -> garder taille fenêtre ou taille SM_CYFULLSCREEN en fullscreen ?
    area.bottom = area.top + height;
    return (SetWindowPos(this->_windowHandle, HWND_TOP, 0, 0, width, height, SWP_NOMOVE) != FALSE);
  }
  
  bool Window::move(uint32_t x, uint32_t y) noexcept {
    RECT area = _getClientArea();
    area.right -= area.left;
    area.bottom -= area.top;
    //TODO - centered / default
    area.left = position.x;
    //TODO - centered / default
    area.top = position.y;
    //TODO AdjustWindowRectExW
    area.right += area.left;
    area.bottom += area.top;

    if (SetWindowPos(this->_windowHandle, HWND_TOP, area.left, area.top, area.right - area.left, area.bottom - area.top, SWP_NOSIZE) != FALSE) {
      _setClientAreaForceRatio(area);
      return true;
    }
    return false;
  }

  bool Window::moveResize(const WindowPosition& position) noexcept {
    RECT area;
    //TODO - centered / default
    area.left = position.x;
    //TODO - centered / default
    area.top = position.y;
    //TODO - default -> garder taille fenêtre ou taille SM_CXFULLSCREEN en fullscreen ?
    area.right = area.left + width;
    //TODO - default -> garder taille fenêtre ou taille SM_CYFULLSCREEN en fullscreen ?
    area.bottom = area.top + height;
    //TODO AdjustWindowRectExW

    return (SetWindowPos(this->_windowHandle, HWND_TOP, area.left, area.top, area.right - area.left, area.bottom - area.top, 0) != FALSE);
  }
#endif
