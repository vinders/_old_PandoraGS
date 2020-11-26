/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>
# include "./window_handle.h"

#ifndef _P_WINDOW_STRING
# ifdef _WINDOWS
#   define __P_STR_WIDEN(x) L ## x
#   define _P_WINDOW_STRING(x) __P_STR_WIDEN(x)
# else
#   define _P_WINDOW_STRING(x) x
# endif
#endif

namespace pandora {
  namespace video {
    class Window;
    class WindowStyleBuilder;

    
    // -- style settings --
    
    using WindowColor = uint32_t;
    
    /// @brief Window display mode
    enum class WindowDisplayMode : uint32_t {
      fullscreen       = 0u, ///< Fullscreen context display
      borderless       = 1u, ///< Borderless popup display (can be used for splash screens or for "fullscreen windows")
      dialog           = 2u, ///< Popup window with caption/menu
      window           = 3u, ///< Normal window with caption/menu/minimization
      floatingWindow   = 4u  ///< Floating toolbox mode (shorter title bar, smaller title font)
    };
    /// @brief Window special settings bits
    /// @warning Some settings may have no effect on certain operating systems
    enum class WindowTweak : uint32_t {
      none           = 0u,   ///< no additional feature
      dropShadow     = 0x1u, ///< Emphasize Z-order with drop shadow (typically for popup dialogs)
      uniqueContext  = 0x4u  ///< Use unique context for window and all sub-children (faster changes, but must be used carefully)
    };
    
    /// @brief Standard system icon type
    enum class SystemIcon : uint32_t {
      defaultApp = 0u, ///< Default application icon
      info       = 1u, ///< Information icon
      question   = 2u, ///< Question mark icon
      warning    = 3u, ///< Warning icon
      error      = 4u, ///< Error icon
      security   = 5u, ///< Security icon
      system     = 6u  ///< System logo
    };

    /// @brief Standard system cursor type
    enum class SystemCursor : uint32_t {
      arrow     = 0u, ///< Standard arrow
      wait      = 1u, ///< Wait cursor (hourglass)
      waitArrow = 2u, ///< Arrow with wait cursor (hourglass)
      hand      = 3u, ///< Hand (pointing)
      help      = 4u, ///< Help indicator (arrow and question mark)
      crosshair = 5u, ///< Crosshair
      textIBeam = 6u, ///< Text I-beam
      forbidden = 7u, ///< Invalid/forbidden (slashed circle...)
      upArrow         = 8u, ///< Vertical arrow
      doubleArrowV    = 9u, ///< Double-pointed resizing arrow (vertical)
      doubleArrowH    = 10u,///< Double-pointed resizing arrow (horizontal)
      doubleArrowNeSw = 11u,///< Double-pointed resizing arrow (oblique N-E / S-W)
      doubleArrowNwSe = 12u,///< Double-pointed resizing arrow (oblique N-W / S-E)
      fourPointArrow  = 13u ///< Four-pointed resizing arrow
    };
    
    
    // -- UI resource management --
    
    /// @class IconResource
    /// @brief RAII icon resource handle. Typically stored in a shared_ptr.
    class IconResource final {
    public:
      IconResource() = default;
      IconResource(ResourceHandle handle) : _handle(handle) {}
      IconResource(const IconResource&) = delete;
      IconResource(IconResource&&) = delete;
      IconResource& operator=(const IconResource&) = delete;
      IconResource& operator=(IconResource&&) = delete;
      ~IconResource() noexcept;
      
      inline operator bool() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline bool isValid() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline ResourceHandle handle() const noexcept { return this->_handle; }
    private:
      ResourceHandle _handle = (ResourceHandle)0;
    };
    /// @class CursorResource
    /// @brief RAII cursor resource handle. Typically stored in a shared_ptr.
    class CursorResource final {
    public:
      CursorResource() = default;
      CursorResource(ResourceHandle handle) : _handle(handle) {}
      CursorResource(const CursorResource&) = delete;
      CursorResource(CursorResource&&) = delete;
      CursorResource& operator=(const CursorResource&) = delete;
      CursorResource& operator=(CursorResource&&) = delete;
      ~CursorResource() noexcept;
      
      inline operator bool() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline bool isValid() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline ResourceHandle handle() const noexcept { return this->_handle; }
    private:
      ResourceHandle _handle = (ResourceHandle)0;
    };
    /// @class MenuResource
    /// @brief RAII menu resource handle. Typically stored in a shared_ptr.
    class MenuResource final {
    public:
      MenuResource() = default;
      MenuResource(ResourceHandle handle) : _handle(handle) {}
      MenuResource(const MenuResource&) = delete;
      MenuResource(MenuResource&&) = delete;
      MenuResource& operator=(const MenuResource&) = delete;
      MenuResource& operator=(MenuResource&&) = delete;
      ~MenuResource() noexcept;
      
      inline operator bool() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline bool isValid() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline ResourceHandle handle() const noexcept { return this->_handle; }
    private:
      ResourceHandle _handle = (ResourceHandle)0;
    };
    /// @class ColorResource
    /// @brief RAII color resource handle. Typically stored in a shared_ptr.
    class ColorResource final {
    public:
      ColorResource() = default;
      ColorResource(ResourceHandle handle) : _handle(handle) {}
      ColorResource(const ColorResource&) = delete;
      ColorResource(ColorResource&&) = delete;
      ColorResource& operator=(const ColorResource&) = delete;
      ColorResource& operator=(ColorResource&&) = delete;
      ~ColorResource() noexcept;
      
      inline operator bool() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline bool isValid() const noexcept { return (this->_handle != (ResourceHandle)0); }
      inline ResourceHandle handle() const noexcept { return this->_handle; }
    private:
      ResourceHandle _handle = (ResourceHandle)0;
    };
    
    
    // -- window style definition class --
    
    /// @brief Container for window style resources (and keep them alive while the style definition exists)
    struct WindowStyleResources final {
      std::shared_ptr<IconResource> appIcon = nullptr;
      std::shared_ptr<IconResource> captionIcon = nullptr;
      std::shared_ptr<CursorResource> cursor = nullptr;
      std::shared_ptr<ColorResource> backgroundColor = nullptr;
      InstanceHandle appInstance = (InstanceHandle)0;
      WindowDisplayMode displayMode = WindowDisplayMode::window;
    };
    
    /// @class WindowStyle
    /// @brief Window style definition, required to create windows with a specific base style. Typically stored in a shared_ptr.
    /// @remarks Create a window style with WindowStyleBuilder, then use it to create one or several Window objects.
    class WindowStyle final {
    public:
      WindowStyle& operator=(const WindowStyle&) = delete;
      WindowStyle& operator=(WindowStyle&& rhs) noexcept 
        : _styleName(std::move(rhs._styleName)), _resources(std::move(rhs._resources)) { 
        rhs._styleName.clear(); 
      }
      WindowStyle& operator=(const WindowStyle&) = delete;
      WindowStyle& operator=(WindowStyle&& rhs) noexcept { 
        this->_styleName = std::move(rhs._styleName); 
        this->_resources = std::move(rhs._resources); 
        rhs._styleName.clear(); 
      }
      ~WindowStyle() noexcept;
      
      const std::string& name() const noexcept { return _styleName; }
    private:
      WindowStyle(const char uniqueStyleName[], WindowStyleResources& resources) 
        : _styleName(uniqueStyleName), _resources(resources) {}
      friend class WindowStyleBuilder;
      friend class Window;

    private:
#     ifdef _WINDOWS
        const std::wstring _styleName;
#     else
        const std::string _styleName;
#     endif
      WindowStyleResources _resources;
    };


    // -- window style definition builder --
    
    /// @class WindowStyleBuilder
    /// @brief Builder utility to create window resources and a common window style that can be used to create some windows
    class WindowStyleBuilder final {
    public:
      /// @brief Create window style builder
      WindowStyleBuilder(InstanceHandle appInstance = (InstanceHandle)0) noexcept;
      WindowStyleBuilder(const WindowStyleBuilder&) = default;
      WindowStyleBuilder(WindowStyleBuilder&&) = default;
      WindowStyleBuilder& operator=(const WindowStyleBuilder&) = default;
      WindowStyleBuilder& operator=(WindowStyleBuilder&&) = default;
      ~WindowStyleBuilder() = default;
      
      // -- colors --
      
      /// @brief Create custom color reference
      static inline WindowColor createColor(uint8_t red, uint8_t green, uint8_t blue) noexcept {
        return ((uint32_t)red | (((uint32_t)green) << 8) | (((uint32_t)blue) << 16));
      }
      /// @brief Get system window color reference
      static inline WindowColor systemWindowColor() noexcept { return 0xFFFFFFFFu; }
      /// @brief Get system window text color reference
      static inline WindowColor systemTextColor() noexcept { return 0xFFFFFFFEu; }
      
      /// @brief Create color style resource
      /// @returns On success: valid color resource. On failure: nullptr.
      static std::shared_ptr<ColorResource> createColorStyle(WindowColor color) noexcept;
      
      // -- menus --
      
      /// @brief Create color style resource
      /// @returns On success: valid menu resource. On failure: nullptr.
      static std::shared_ptr<MenuResource> createMenu() noexcept;
      
      // -- icons / cursors --
      
      /// @brief Create standard system icon resource
      /// @returns On success: valid icon resource. On failure: nullptr.
      static std::shared_ptr<IconResource> createSystemIcon(SystemIcon id) noexcept;
      /// @brief Read an external icon file and create an icon resource
      /// @warning If width or height equals 0, the default system icon size will be used.
      /// @returns On success: valid icon resource. On failure: nullptr.
      static std::shared_ptr<IconResource> createIconFromFile(const char uri[], uint32_t width = 0u, uint32_t height = 0u) noexcept;
#     ifdef _WINDOWS
        static std::shared_ptr<IconResource> createIconFromFile(const wchar_t uri[], uint32_t width = 0u, uint32_t height = 0u) noexcept;
#     endif

      /// @brief Create standard system cursor resource
      /// @returns On success: valid cursor resource. On failure: nullptr.
      static std::shared_ptr<CursorResource> createSystemCursor(SystemCursor id) noexcept;
      /// @brief Read an external cursor file and create a cursor resource
      /// @returns On success: valid cursor resource. On failure: nullptr.
      static std::shared_ptr<CursorResource> createCursorFromFile(const char uri[]) noexcept;
#     ifdef _WINDOWS
        static std::shared_ptr<CursorResource> createCursorFromFile(const wchar_t uri[]) noexcept;
#     endif
      
#     ifdef _WINDOWS
        /// @brief Read an embedded icon (referenced in .rc file) and create an icon resource
        /// @returns On success: valid icon resource. On failure: nullptr.
        std::shared_ptr<IconResource> readInternalIcon(const char id[]) const noexcept;
        std::shared_ptr<IconResource> readInternalIcon(const wchar_t id[]) const noexcept;
        /// @brief Read an embedded cursor (referenced in .rc file) and create a cursor resource
        /// @returns On success: valid cursor resource. On failure: nullptr.
        std::shared_ptr<CursorResource> readInternalCursor(const char id[]) const noexcept;
        std::shared_ptr<CursorResource> readInternalCursor(const wchar_t id[]) const noexcept;
#     else
        /// @brief Read an icon with standard size located in the same directory as the application and create an icon resource
        /// @returns On success: valid icon resource. On failure: nullptr.
        std::shared_ptr<IconResource> readInternalIcon(const char fileName[]) const noexcept;
        /// @brief Read a cursor with standard size located in the same directory as the application and create a cursor resource
        /// @returns On success: valid cursor resource. On failure: nullptr.
        std::shared_ptr<CursorResource> readInternalCursor(const char fileName[]) const noexcept;
#     endif
      
      // -- build window style --
      
      /// @brief Set initial display mode of the window
      inline WindowStyleBuilder& setDisplayMode(WindowDisplayMode displayMode) noexcept { this->_resources.displayMode = displayMode; return *this; }
      /// @brief Set special settings of the window
      /// @warning Some settings may have no effect on certain operating systems
      inline WindowStyleBuilder& setSpecialTweaks(WindowTweak tweaks) noexcept { this->_tweaks = tweaks; return *this; }
      /// @brief Set background color of the window
      inline WindowStyleBuilder& setBackgroundColor(std::shared_ptr<ColorResource> backgroundColor) noexcept { this->_resources.backgroundColor = backgroundColor; return *this; }
      
      /// @brief Set main application icon (taskbar, shortcut...), or set nullptr to use default
      inline WindowStyleBuilder& setAppIcon(std::shared_ptr<IconResource> appIcon) noexcept { this->_resources.appIcon = appIcon; return *this; }
      /// @brief Set caption icon (if omitted, the application icon will be used), or set nullptr to use default
      inline WindowStyleBuilder& setCaptionIcon(std::shared_ptr<IconResource> captionIcon) noexcept { this->_resources.captionIcon = captionIcon; return *this; }
      /// @brief Set main application icon (taskbar, shortcut...), or set nullptr to use default
      inline WindowStyleBuilder& setDefaultCursor(std::shared_ptr<CursorResource> cursor) noexcept { this->_resources.cursor = cursor; return *this; }
      
      /// @brief Build window style based on the current settings of the builder (last method called)
      /// @warning The style name must be a unique identifier
      /// @returns On success: style instance to provide to Window constructor(s). On failure: nullptr.
#     ifdef _WINDOWS
        std::shared_ptr<WindowStyle> build(const wchar_t uniqueStyleName[]) noexcept;
#     else
        std::shared_ptr<WindowStyle> build(const char uniqueStyleName[]) noexcept;
#     endif
      
    private:
      WindowStyleResources _resources;
      WindowTweak _tweaks = WindowTweak::none;
    };
    
  }
}
