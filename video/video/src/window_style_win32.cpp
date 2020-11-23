/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : Window style definition & resources management: Win32 implementation (Windows)
*******************************************************************************/
#ifdef _WINDOWS
# include <cstdint>
# include <cstddef>
# include <cstring>
# include <system/api/windows_api.h>
# include <windowsx.h>
# include "video/window_handle.h"
# include "video/window_style.h"

  using namespace pandora::video;
  
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

  // -- menu resource management --
  
  std::shared_ptr<MenuResource> WindowStyleBuilder::createMenu() noexcept {
    ResourceHandle handle = (ResourceHandle)CreateMenu();
    return (handle != nullptr) ? std::make_shared<MenuResource>(handle) : nullptr;
  }
  
  //bool MenuResource::insertItem(MENUITEMINFOW& menuInfo, int32_t index = -1) noexcept { 
  //  return (InsertMenuItemW(this->_handle, index, TRUE, &menuInfo) != FALSE); 
  //}
  //bool MenuResource::insertSubMenu(std::shared_ptr<MenuResource> subMenu, const wchar_t label[], int32_t index = -1, bool isDisabled = false, bool hasLeftSeparator = false) noexcept { 
  //  UINT flags = isDisabled ? (MF_DISABLED | MF_GRAYED) : MF_ENABLED;
  //  if (hasLeftSeparator)
  //    flags |= MF_MENUBARBREAK;
  //  return (InsertMenuW(this->_handle, index, (flags | MF_BYPOSITION | MF_STRING | MF_POPUP), subMenu->_handle, label) != FALSE); 
  //}
  //bool MenuResource::insertSubMenu(std::shared_ptr<MenuResource> subMenu, std::shared_ptr<BitmapResource> image, int32_t index = -1, bool isDisabled = false, bool hasLeftSeparator = false) noexcept { 
  //  UINT flags = isDisabled ? (MF_DISABLED | MF_GRAYED) : MF_ENABLED;
  //  if (hasLeftSeparator)
  //    flags |= MF_MENUBARBREAK;
  //  return (InsertMenuW(this->_handle, index, (flags | MF_BYPOSITION | MF_BITMAP | MF_POPUP), subMenu->_handle, image->_handle) != FALSE); 
  //}
  //bool MenuResource::insertHorizontalSeparator(int32_t index = -1) noexcept { 
  //  return (InsertMenuW(this->_handle, index, (MF_BYPOSITION | MF_SEPARATOR), 0, nullptr) != FALSE); 
  //}
  
  MenuResource::~MenuResource() noexcept { 
    if (this->_handle != nullptr)
      DestroyMenu((HMENU)this->_handle); 
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
    windowClass.cbWndExtra = (this->_resources.displayMode != WindowDisplayMode::dialog) ? 0 : DLGWINDOWEXTRA;
    windowClass.lpfnWndProc = Window::windowEventHandler;
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
#endif
