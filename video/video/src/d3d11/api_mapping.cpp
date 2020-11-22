/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#include "video/d3d11/api_mapping.h"

using namespace pandora::video;
using pandora::video::d3d11::ApiMapping;

// -- drawing --

D3D11_CULL_MODE ApiMapping::toCullingMode(CullingMode mode) noexcept {
  switch (mode) {
    case CullingMode::none:  return D3D11_CULL_NONE;
    case CullingMode::front: return D3D11_CULL_FRONT;
    case CullingMode::back:  return D3D11_CULL_BACK;
    default: return D3D11_CULL_NONE;
  }
}

D3D_SRV_DIMENSION ApiMapping::toTextureDimension(TextureDimension type) noexcept {
  switch (type) {
    case TextureDimension::unknown:        return D3D11_SRV_DIMENSION_UNKNOWN;
    case TextureDimension::buffer:         return D3D11_SRV_DIMENSION_BUFFER;
    case TextureDimension::rawBuffer:      return D3D11_SRV_DIMENSION_BUFFEREX;
    case TextureDimension::texture1d:      return D3D11_SRV_DIMENSION_TEXTURE1D;
    case TextureDimension::texture1dArray: return D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
    case TextureDimension::texture2d:      return D3D11_SRV_DIMENSION_TEXTURE2D;
    case TextureDimension::texture2dArray: return D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    case TextureDimension::textureMulti2d: return D3D11_SRV_DIMENSION_TEXTURE2DMS;
    case TextureDimension::textureMulti2dArray: return D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
    case TextureDimension::texture3d:      return D3D11_SRV_DIMENSION_TEXTURE3D;
    case TextureDimension::textureCubeMap: return D3D11_SRV_DIMENSION_TEXTURECUBE;
    case TextureDimension::textureCubeMapArray: return D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
    default: return D3D11_SRV_DIMENSION_UNKNOWN;
  }
}

D3D11_TEXTURE_ADDRESS_MODE ApiMapping::toTextureAddressMode(TextureAddressMode mode) noexcept {
  switch (mode) {
    case TextureAddressMode::wrap:   return D3D11_TEXTURE_ADDRESS_WRAP;
    case TextureAddressMode::mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
    case TextureAddressMode::clamp:  return D3D11_TEXTURE_ADDRESS_CLAMP;
    case TextureAddressMode::border: return D3D11_TEXTURE_ADDRESS_BORDER;
    default: return D3D11_TEXTURE_ADDRESS_WRAP;
  }
}

// -- operations --
      
D3D11_STENCIL_OP ApiMapping::toStencilOperation(StencilOperation op) noexcept {
  switch (op) {
    case StencilOperation::keep:           return D3D11_STENCIL_OP_KEEP;
    case StencilOperation::zero:           return D3D11_STENCIL_OP_ZERO;
    case StencilOperation::invert:         return D3D11_STENCIL_OP_INVERT;
    case StencilOperation::replace:        return D3D11_STENCIL_OP_REPLACE;
    case StencilOperation::incrementClamp: return D3D11_STENCIL_OP_INCR_SAT;
    case StencilOperation::decrementClamp: return D3D11_STENCIL_OP_DECR_SAT;
    case StencilOperation::incrementWrap:  return D3D11_STENCIL_OP_INCR;
    case StencilOperation::decrementWrap:  return D3D11_STENCIL_OP_DECR;
    default: return D3D11_STENCIL_OP_KEEP;
  }
}

D3D11_BLEND_OP ApiMapping::toBlendOperation(BlendOperation op) noexcept {
  switch (op) {
    case BlendOperation::add:             return D3D11_BLEND_OP_ADD;
    case BlendOperation::subtract:        return D3D11_BLEND_OP_SUBTRACT;
    case BlendOperation::reverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
    case BlendOperation::min:             return D3D11_BLEND_OP_MIN;
    case BlendOperation::max:             return D3D11_BLEND_OP_MAX;
    default: return D3D11_BLEND_OP_ADD;
  }
}

D3D11_BLEND ApiMapping::toBlendFactor(BlendFactor mode) noexcept {
  switch (mode) {
    case BlendFactor::zero:            return D3D11_BLEND_ZERO;
    case BlendFactor::one:             return D3D11_BLEND_ONE;
    case BlendFactor::sourceColor:     return D3D11_BLEND_SRC_COLOR;
    case BlendFactor::sourceColorInv:  return D3D11_BLEND_INV_SRC_COLOR;
    case BlendFactor::sourceAlpha:     return D3D11_BLEND_SRC_ALPHA;
    case BlendFactor::sourceAlphaInv:  return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendFactor::destColor:       return D3D11_BLEND_DEST_COLOR;
    case BlendFactor::destColorInv:    return D3D11_BLEND_INV_DEST_COLOR;
    case BlendFactor::destAlpha:       return D3D11_BLEND_DEST_ALPHA;
    case BlendFactor::destAlphaInv:    return D3D11_BLEND_INV_DEST_ALPHA;
    case BlendFactor::alphaToColors:   return D3D11_BLEND_SRC_ALPHA_SAT;
    case BlendFactor::constant:        return D3D11_BLEND_BLEND_FACTOR;
    case BlendFactor::constantInv:     return D3D11_BLEND_INV_BLEND_FACTOR;
    case BlendFactor::source1Color:    return D3D11_BLEND_SRC1_COLOR;
    case BlendFactor::source1ColorInv: return D3D11_BLEND_INV_SRC1_COLOR;
    case BlendFactor::source1Alpha:    return D3D11_BLEND_SRC1_ALPHA;
    case BlendFactor::source1AlphaInv: return D3D11_BLEND_INV_SRC1_ALPHA;
    default: return D3D11_BLEND_ZERO;
  }
}

// -- colors --

// pixel format mapping: {id}
DXGI_FORMAT ApiMapping::_formats[(uint32_t)pandora::video::PixelFormat::end + 1u] = {
  DXGI_FORMAT_UNKNOWN,
      
  // -- standard HDR RGB --
  
  DXGI_FORMAT_R32G32B32A32_TYPELESS,
  DXGI_FORMAT_R32G32B32A32_FLOAT,
  DXGI_FORMAT_R32G32B32A32_UINT,
  DXGI_FORMAT_R32G32B32A32_SINT,
  DXGI_FORMAT_R32G32B32_TYPELESS,
  DXGI_FORMAT_R32G32B32_FLOAT,
  DXGI_FORMAT_R32G32B32_UINT,
  DXGI_FORMAT_R32G32B32_SINT,
  
  DXGI_FORMAT_R16G16B16A16_TYPELESS,
  DXGI_FORMAT_R16G16B16A16_FLOAT,
  DXGI_FORMAT_R16G16B16A16_UINT,
  DXGI_FORMAT_R16G16B16A16_SINT,
  DXGI_FORMAT_R16G16B16A16_UNORM,
  DXGI_FORMAT_R16G16B16A16_SNORM,
  
  // -- standard RGB --
  
  DXGI_FORMAT_R8G8B8A8_TYPELESS,
  DXGI_FORMAT_R8G8B8A8_UINT,
  DXGI_FORMAT_R8G8B8A8_SINT,
  DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
  DXGI_FORMAT_R8G8B8A8_UNORM,
  DXGI_FORMAT_R8G8B8A8_SNORM,
  
  DXGI_FORMAT_B8G8R8A8_TYPELESS,
  DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
  DXGI_FORMAT_B8G8R8A8_UNORM,
  
  // -- packed RGB --
  
  DXGI_FORMAT_R10G10B10A2_TYPELESS,
  DXGI_FORMAT_R10G10B10A2_UNORM,
  DXGI_FORMAT_R10G10B10A2_UINT,
  DXGI_FORMAT_R11G11B10_FLOAT,
  DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
  
  DXGI_FORMAT_B5G6R5_UNORM,
  DXGI_FORMAT_B5G5R5A1_UNORM,
  DXGI_FORMAT_B4G4R4A4_UNORM,

  // -- components --
  
  DXGI_FORMAT_R32G32_TYPELESS,
  DXGI_FORMAT_R32G32_FLOAT,
  DXGI_FORMAT_R32G32_UINT,
  DXGI_FORMAT_R32G32_SINT,
  DXGI_FORMAT_R16G16_TYPELESS,
  DXGI_FORMAT_R16G16_FLOAT,
  DXGI_FORMAT_R16G16_UINT,
  DXGI_FORMAT_R16G16_SINT,
  DXGI_FORMAT_R16G16_UNORM,
  DXGI_FORMAT_R16G16_SNORM,
  DXGI_FORMAT_R8G8_TYPELESS,
  DXGI_FORMAT_R8G8_UINT,
  DXGI_FORMAT_R8G8_SINT,
  DXGI_FORMAT_R8G8_UNORM,
  DXGI_FORMAT_R8G8_SNORM,
  
  DXGI_FORMAT_R32_TYPELESS,
  DXGI_FORMAT_R32_FLOAT,
  DXGI_FORMAT_R32_UINT,
  DXGI_FORMAT_R32_SINT,
  DXGI_FORMAT_R16_TYPELESS,
  DXGI_FORMAT_R16_FLOAT,
  DXGI_FORMAT_R16_UINT,
  DXGI_FORMAT_R16_SINT,
  DXGI_FORMAT_R16_UNORM,
  DXGI_FORMAT_R16_SNORM,
  DXGI_FORMAT_R8_TYPELESS,
  DXGI_FORMAT_R8_UINT,
  DXGI_FORMAT_R8_SINT,
  DXGI_FORMAT_R8_UNORM,
  DXGI_FORMAT_R8_SNORM,
  DXGI_FORMAT_A8_UNORM,

  DXGI_FORMAT_UNKNOWN
};
