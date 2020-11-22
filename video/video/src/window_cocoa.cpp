/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window management: Cocoa implementation (Mac OS)
*******************************************************************************/
#if !defined(_WINDOWS)
# include <system/operating_system.h>
#endif
#if defined(__APPLE__) && defined(_SYSTEM_OPERATING_SYSTEM) && _SYSTEM_OPERATING_SYSTEM != _SYSTEM_OS_IOS
# include <cstdint>
# include <cstddef>
# include <cstring>
# include "video/window_style.h"
# include "video/window.h"

using namespace pandora::video;

// -- display/size/style native enums -- ---------------------------------------



// -- native resource management -- --------------------------------------------

  // -- icon resource management --

  std::shared_ptr<IconResource> WindowStyleBuilder::createSystemIcon(SystemIcon id) noexcept {
    return nullptr;
  }
  std::shared_ptr<IconResource> WindowStyleBuilder::createIconFromFile(const char uri[], uint32_t width, uint32_t height) noexcept {
    return nullptr;
  }
  std::shared_ptr<IconResource> WindowStyleBuilder::readInternalIcon(const char id[]) const noexcept {
    return nullptr;
  }

  IconResource::~IconResource() noexcept { 
  }

  // -- cursor resource management --

  std::shared_ptr<CursorResource> WindowStyleBuilder::createSystemCursor(SystemCursor id) noexcept {
    return nullptr;
  }
  std::shared_ptr<CursorResource> WindowStyleBuilder::createCursorFromFile(const char uri[]) noexcept {
    return nullptr;
  }
  std::shared_ptr<CursorResource> WindowStyleBuilder::readInternalCursor(const char id[]) const noexcept {
    return nullptr;
  }

  CursorResource::~CursorResource() noexcept { 
    if (this->_handle != nullptr)
      DestroyCursor((HCURSOR)this->_handle); 
  }

  // -- color resource management --

  std::shared_ptr<ColorResource> WindowStyleBuilder::createColorStyle(WindowColor color) noexcept {
    return nullptr;
  }
  ColorResource::~ColorResource() noexcept { 
  }


// -- window event processing -- -----------------------------------------------



// -- window definition class creation/destruction -- --------------------------

  WindowStyleBuilder::WindowStyleBuilder(InstanceHandle appInstance) noexcept {
  }

  // create style definition class
  std::shared_ptr<WindowStyle> WindowStyleBuilder::build(const char uniqueStyleName[]) noexcept {
    if (uniqueStyleName == nullptr || *uniqueStyleName == static_cast<char>(0))
      return nullptr;
    
    return nullptr;
  }
  
  // destroy style definition class
  WindowStyle::~WindowStyle() noexcept {
  }


// -- window metrics -- --------------------------------------------------------

  

// -- window creation/destruction -- -------------------------------------------

  // create window
  Window::Window(const WindowPosition& position, std::shared_ptr<WindowStyle> windowStyleDef, WindowWidgetMode widgets, WindowSizeMode sizeMode,
                 const char caption[], bool isCursorVisible, WindowVisibility initialStatus, std::shared_ptr<MenuResource> menu, WindowHandle owner)
    : _windowStyleDef(std::move(windowStyleDef)),
      _menu(std::move(menu)),
      _widgets(widgets),
      _sizeMode(sizeMode),
      _isCursorVisible(isCursorVisible) {
    this->_backgroundColor = this->_windowStyleDef.backgroundColor;
    this->_displayMode = this->_windowStyleDef->displayMode;

  }
  
  // destroy window
  Window::~Window() noexcept {
  }
  
  // -- accessors --
  
  WindowPixel Window::clientAreaSize() const noexcept {
    std::lock_guard<pandora::thread::SpinLock> lock(_clientAreaLock);
    return { 0, 0 };
  }
  WindowPixel Window::cursorScreenPosition() noexcept {
    return { -1, -1 };
  }
  WindowPixel Window::cursorWindowPosition() noexcept {
    return { -1, -1 };
  }

  // -- change display settings --

  bool Window::displayMode(WindowDisplayMode displayMode) noexcept {
    return false;
  }
  bool Window::displayMode(WindowDisplayMode displayMode, WindowSizeMode sizeMode) noexcept {
    return false;
  } 
  bool Window::sizeMode(WindowSizeMode sizeMode) noexcept {
    return false;
  }

  bool Window::visibility(WindowVisibility status) noexcept {
    return false;
  }

  bool Window::cursorType(std::shared_ptr<CursorResource> cursor) noexcept {
    return false;
  }
  bool Window::cursorScreenPosition(uint32_t absoluteX, uint32_t absoluteY) noexcept {
    return false;
  }
  bool Window::cursorWindowPosition(uint32_t clientAreaX, uint32_t clientAreaY) noexcept {
    return false;
  }
  void Window::cursorVisibility(bool isVisible) noexcept {
  }

  bool Window::caption(const char caption[]) noexcept {
    return false;
  }
  
  bool Window::menuType(std::shared_ptr<MenuResource> menu) noexcept {
    return false;
  }

  void Window::clearClientArea() noexcept {
  }
  void Window::clearClientArea(const WindowPosition& position) noexcept {
  }
  
  void Window::fillClientArea(std::shared_ptr<ColorResource> color) noexcept {
  }
  void Window::fillClientArea(const WindowPosition& position, std::shared_ptr<ColorResource> color) noexcept{
  }

  // -- change size / position --

  bool Window::resize(uint32_t width, uint32_t height) noexcept {
    return false;
  }
  
  bool Window::move(uint32_t x, uint32_t y) noexcept {
    return false;
  }

  bool Window::moveResize(const WindowPosition& position) noexcept {
    return false;
  }
#endif
