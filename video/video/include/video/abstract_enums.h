/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
--------------------------------------------------------------------------------
Description : API abstraction layer: configuration enumerations
*******************************************************************************/
#pragma once

#include <cstdint>

namespace pandora {
  namespace video {
    // -- drawing --
    
    /// @brief Polygon culling mode
    enum class CullingMode : uint32_t {
      none = 0u, ///< Draw every polygon
      front,     ///< Draw only "front" polygons (vertex order: defined in rasterizer)
      back       ///< Draw only "back" polygons (vertex order: defined in rasterizer)
    };
    
    /// @brief Type of resources viewed by shaders
    enum class TextureDimension : uint32_t {
      unknown = 0u,
      buffer,              ///< Simple buffer
      rawBuffer,           ///< Raw buffer
      texture1d,           ///< 1D texture
      texture1dArray,      ///< array of 1D textures
      texture2d,           ///< 2D texture
      texture2dArray,      ///< array of 2D textures
      textureMulti2d,      ///< Multisample 2D texture
      textureMulti2dArray, ///< array of multisample 2D textures
      texture3d,           ///< 3D texture
      textureCubeMap,      ///< Cube texture
      textureCubeMapArray  ///< Array of cube textures
    };
    /// @brief Texture addressing/repeating modes
    enum class TextureAddressMode : uint32_t {
      wrap = 0u, ///< Repeat
      mirror,    ///< Mirrored repetitions
      clamp,     ///< Copy edge pixels
      border     ///< Use a border color for coordinates out of the range [0;1]
    };
    
    
    // -- operations --
    
    /// @brief Stencil operations performed during depth-stencil testing
    enum class StencilOperation : uint32_t {
      keep,           ///< Keep existing data
      zero,           ///< Set data to 0
      invert,         ///< Invert stencil data
      replace,        ///< Replace by reference stencil state
      incrementClamp, ///< Increment by 1 + clamp result
      decrementClamp, ///< Decrement by 1 + clamp result
      incrementWrap,  ///< Increment by 1 + wrap result
      decrementWrap   ///< Decrement by 1 + wrap result
    };
    
    /// @brief Color/alpha blending operations
    enum class BlendOperation: uint32_t {
      add = 0u,        ///< source1 + source2
      subtract,        ///< source2 - source1
      reverseSubtract, ///< source1 - source2
      min,             ///< min(source1, source2)
      max              ///< max(source1, source2)
    };
    /// @brief Factor modes for blending operations
    enum class BlendFactor : uint32_t {
      zero = 0u,           ///< (0, 0, 0, 0)
      one,                 ///< (1, 1, 1, 1)
      sourceColor,         ///< (sR, sG, sB, sA)
      sourceColorInv, ///< (1 - sR, 1 - sG, 1 - sB, 1 - sA)
      sourceAlpha,         ///< (sA, sA, sA, sA)
      sourceAlphaInv, ///< (1 - sA, 1 - sA, 1 - sA, 1 - sA)
      destColor,           ///< (dR, dG, dB, dA)
      destColorInv,   ///< (1 - dR, 1 - dG, 1 - dB, 1 - dA)
      destAlpha,           ///< (dA, dA, dA, dA)
      destAlphaInv,   ///< (1 - dA, 1 - dA, 1 - dA, 1 - dA)
      alphaToColors,       ///< (f, f, f, 1) with f=min(sA,1-dA) for Direct3D / f=min(sA,kA−dA)/kA for OpenGL
      constant,            ///< constant blend factor defined in device context
      constantInv,    ///< 1 - constant blend factor defined in device context
      source1Color,        ///< multi-output: source color
      source1ColorInv,///< multi-output: 1 - source color
      source1Alpha,        ///< multi-output: source alpha
      source1AlphaInv ///< multi-output: 1 - source alpha
    };
    
    
    // -- colors --
    
    /// @brief Pixel color format types (commented values: Direct3D / OpenGL)
    enum class PixelFormat : uint32_t {
      unknown = 0u,     ///< DXGI_FORMAT_UNKNOWN / GL_NONE
      
      // -- standard HDR RGB --
      
      rgba32_none,      ///< DXGI_FORMAT_R32G32B32A32_TYPELESS / GL_RGBA32UI(GL_NONE)
      rgba32_float,     ///< DXGI_FORMAT_R32G32B32A32_FLOAT / GL_RGBA32F
      rgba32_uint,      ///< DXGI_FORMAT_R32G32B32A32_UINT / GL_RGBA32UI
      rgba32_int,       ///< DXGI_FORMAT_R32G32B32A32_SINT / GL_RGBA32I
      rgb32_none,       ///< DXGI_FORMAT_R32G32B32_TYPELESS / GL_RGB32UI(GL_NONE)
      rgb32_float,      ///< DXGI_FORMAT_R32G32B32_FLOAT / GL_RGB32F
      rgb32_uint,       ///< DXGI_FORMAT_R32G32B32_UINT / GL_RGB32UI
      rgb32_int,        ///< DXGI_FORMAT_R32G32B32_SINT / GL_RGB32I
      
      rgba16_none,      ///< DXGI_FORMAT_R16G16B16A16_TYPELESS / GL_RGBA16(GL_NONE)
      rgba16_float,     ///< DXGI_FORMAT_R16G16B16A16_FLOAT / GL_RGBA16F
      rgba16_uint,      ///< DXGI_FORMAT_R16G16B16A16_UINT / GL_RGBA16UI
      rgba16_int,       ///< DXGI_FORMAT_R16G16B16A16_SINT / GL_RGBA16I
      rgba16_unorm,     ///< DXGI_FORMAT_R16G16B16A16_UNORM / GL_RGBA16
      rgba16_norm,      ///< DXGI_FORMAT_R16G16B16A16_SNORM / GL_RGBA16_SNORM
      
      // -- standard RGB --
      
      rgba8_none,       ///< DXGI_FORMAT_R8G8B8A8_TYPELESS / GL_RGBA8(GL_NONE)
      rgba8_uint,       ///< DXGI_FORMAT_R8G8B8A8_UINT / GL_RGBA8UI
      rgba8_int,        ///< DXGI_FORMAT_R8G8B8A8_SINT / GL_RGBA8I
      rgba8_unorm_srgb, ///< DXGI_FORMAT_R8G8B8A8_UNORM_SRGB / GL_SRGB8_ALPHA8
      rgba8_unorm,      ///< DXGI_FORMAT_R8G8B8A8_UNORM / GL_RGBA8
      rgba8_snorm,      ///< DXGI_FORMAT_R8G8B8A8_SNORM / GL_RGBA8_SNORM
      
      bgra8_none,       ///< DXGI_FORMAT_B8G8R8A8_TYPELESS / GL_RGBA8(GL_NONE)
      bgra8_unorm_srgb, ///< DXGI_FORMAT_B8G8R8A8_UNORM_SRGB / GL_SRGB8_ALPHA8
      bgra8_unorm,      ///< DXGI_FORMAT_B8G8R8A8_UNORM / GL_RGBA8
      
      // -- packed RGB --
      
      rgb10a2_none,     ///< DXGI_FORMAT_R10G10B10A2_TYPELESS / GL_RGB10_A2(GL_NONE)
      rgb10a2_unorm,    ///< DXGI_FORMAT_R10G10B10A2_UNORM / GL_RGB10_A2
      rgb10a2_uint,     ///< DXGI_FORMAT_R10G10B10A2_UINT / GL_RGB10_A2UI
      rg11b10_float,    ///< DXGI_FORMAT_R11G11B10_FLOAT / GL_R11F_G11F_B10F
      rgb9e5,           ///< DXGI_FORMAT_R9G9B9E5_SHAREDEXP / GL_RGB9_E5
      
      b5g6r5_unorm,     ///< DXGI_FORMAT_B5G6R5_UNORM / GL_RGB565
      bgr5a1_unorm,     ///< DXGI_FORMAT_B5G5R5A1_UNORM / GL_RGB5_A1
      bgra4_unorm,      ///< DXGI_FORMAT_B4G4R4A4_UNORM / GL_RGBA4

      // -- components --
      
      rg32_none,        ///< DXGI_FORMAT_R32G32_TYPELESS / GL_RG32UI(GL_NONE)
      rg32_float,       ///< DXGI_FORMAT_R32G32_FLOAT / GL_RG32F
      rg32_uint,        ///< DXGI_FORMAT_R32G32_UINT / GL_RG32UI
      rg32_int,         ///< DXGI_FORMAT_R32G32_SINT / GL_RG32I
      rg16_none,        ///< DXGI_FORMAT_R16G16_TYPELESS / GL_RG16(GL_NONE)
      rg16_float,       ///< DXGI_FORMAT_R16G16_FLOAT / GL_RG16F
      rg16_uint,        ///< DXGI_FORMAT_R16G16_UINT / GL_RG16UI
      rg16_int,         ///< DXGI_FORMAT_R16G16_SINT / GL_RG16I
      rg16_unorm,       ///< DXGI_FORMAT_R16G16_UNORM / GL_RG16
      rg16_norm,        ///< DXGI_FORMAT_R16G16_SNORM / GL_RG16_SNORM
      rg8_none,         ///< DXGI_FORMAT_R8G8_TYPELESS / GL_RG8(GL_NONE)
      rg8_uint,         ///< DXGI_FORMAT_R8G8_UINT / GL_RG8UI
      rg8_int,          ///< DXGI_FORMAT_R8G8_SINT / GL_RG8I
      rg8_unorm,        ///< DXGI_FORMAT_R8G8_UNORM / GL_RG8
      rg8_norm,         ///< DXGI_FORMAT_R8G8_SNORM / GL_RG8_SNORM
      
      r32_none,         ///< DXGI_FORMAT_R32_TYPELESS / GL_R32UI(GL_NONE)
      r32_float,        ///< DXGI_FORMAT_R32_FLOAT / GL_R32F
      r32_uint,         ///< DXGI_FORMAT_R32_UINT / GL_R32UI
      r32_int,          ///< DXGI_FORMAT_R32_SINT / GL_R32I
      r16_none,         ///< DXGI_FORMAT_R16_TYPELESS / GL_R16(GL_NONE)
      r16_float,        ///< DXGI_FORMAT_R16_FLOAT / GL_R16F
      r16_uint,         ///< DXGI_FORMAT_R16_UINT / GL_R16UI
      r16_int,          ///< DXGI_FORMAT_R16_SINT / GL_R16I
      r16_unorm,        ///< DXGI_FORMAT_R16_UNORM / GL_R16
      r16_norm,         ///< DXGI_FORMAT_R16_SNORM / GL_R16_SNORM
      r8_none,          ///< DXGI_FORMAT_R8_TYPELESS / GL_R8(GL_NONE)
      r8_uint,          ///< DXGI_FORMAT_R8_UINT / GL_R8UI
      r8_int,           ///< DXGI_FORMAT_R8_SINT / GL_R8I
      r8_unorm,         ///< DXGI_FORMAT_R8_UNORM / GL_R8
      r8_norm,          ///< DXGI_FORMAT_R8_SNORM / GL_R8_SNORM
      alpha8_unorm,     ///< DXGI_FORMAT_A8_UNORM / GL_ALPHA8

      end // end of enum (for mapping arrays)
    };
  }
}
