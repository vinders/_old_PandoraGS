/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#include "video/openGL4/api_mapping.h"

using namespace pandora::video;
using pandora::video::openGL4::ApiMapping;

// -- drawing --

GLenum ApiMapping::toCullingMode(CullingMode mode) noexcept {
  switch (mode) {
    case CullingMode::none:  return GL_FRONT_AND_BACK;
    case CullingMode::front: return GL_FRONT;
    case CullingMode::back:  return GL_BACK;
    default: return GL_FRONT_AND_BACK;
  }
}

GLenum ApiMapping::toTextureDimension(TextureDimension type) noexcept {
  switch (type) {
    case TextureDimension::unknown:        return GL_NONE;
    case TextureDimension::buffer:         return GL_TEXTURE_BUFFER;
    case TextureDimension::rawBuffer:      return GL_TEXTURE_BUFFER;
    case TextureDimension::texture1d:      return GL_TEXTURE_1D;
    case TextureDimension::texture1dArray: return GL_TEXTURE_1D_ARRAY;
    case TextureDimension::texture2d:      return GL_TEXTURE_2D;
    case TextureDimension::texture2dArray: return GL_TEXTURE_2D_ARRAY;
    case TextureDimension::textureMulti2d: return GL_TEXTURE_2D_MULTISAMPLE;
    case TextureDimension::textureMulti2dArray: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    case TextureDimension::texture3d:      return GL_TEXTURE_3D;
    case TextureDimension::textureCubeMap: return GL_TEXTURE_CUBE_MAP;
    case TextureDimension::textureCubeMapArray: return GL_TEXTURE_CUBE_MAP_ARRAY;
    default: return GL_NONE;
  }
}

GLenum ApiMapping::toTextureAddressMode(TextureAddressMode mode) noexcept {
  switch (mode) {
    case TextureAddressMode::wrap:   return GL_REPEAT;
    case TextureAddressMode::mirror: return GL_MIRRORED_REPEAT;
    case TextureAddressMode::clamp:  return GL_CLAMP_TO_EDGE;
    case TextureAddressMode::border: return GL_CLAMP_TO_BORDER;
    default: return GL_REPEAT;
  }
}

// -- operations --
      
GLenum ApiMapping::toStencilOperation(StencilOperation op) noexcept {
  switch (op) {
    case StencilOperation::keep:           return GL_KEEP;
    case StencilOperation::zero:           return GL_ZERO;
    case StencilOperation::invert:         return GL_INVERT;
    case StencilOperation::replace:        return GL_REPLACE;
    case StencilOperation::incrementClamp: return GL_INCR;
    case StencilOperation::decrementClamp: return GL_DECR;
    case StencilOperation::incrementWrap:  return GL_INCR_WRAP;
    case StencilOperation::decrementWrap:  return GL_DECR_WRAP;
    default: return GL_KEEP;
  }
}

GLenum ApiMapping::toBlendOperation(BlendOperation op) noexcept {
  switch (op) {
    case BlendOperation::add:             return GL_FUNC_ADD;
    case BlendOperation::subtract:        return GL_FUNC_SUBTRACT;
    case BlendOperation::reverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
    case BlendOperation::min:             return GL_MIN;
    case BlendOperation::max:             return GL_MAX;
    default: return GL_FUNC_ADD;
  }
}

GLenum ApiMapping::toBlendFactor(BlendFactor mode) noexcept {
  switch (mode) {
    case BlendFactor::zero:            return GL_ZERO;
    case BlendFactor::one:             return GL_ONE;
    case BlendFactor::sourceColor:     return GL_SRC_COLOR;
    case BlendFactor::sourceColorInv:  return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::sourceAlpha:     return GL_SRC_ALPHA;
    case BlendFactor::sourceAlphaInv:  return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::destColor:       return GL_DST_COLOR;
    case BlendFactor::destColorInv:    return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::destAlpha:       return GL_DST_ALPHA;
    case BlendFactor::destAlphaInv:    return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::alphaToColors:   return GL_SRC_ALPHA_SATURATE;
    case BlendFactor::constant:        return GL_CONSTANT_COLOR;
    case BlendFactor::constantInv:     return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::source1Color:    return GL_SRC1_COLOR;
    case BlendFactor::source1ColorInv: return GL_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::source1Alpha:    return GL_SRC1_ALPHA;
    case BlendFactor::source1AlphaInv: return GL_ONE_MINUS_SRC1_ALPHA;
    default: return GL_ZERO;
  }
}

// -- colors --

// pixel format mapping: {components, type, format}
pandora::video::openGL4::GLPixelFormat pandora::video::openGL4::ApiMapping::_formats[(uint32_t)pandora::video::PixelFormat::end + 1u] = {
  {GL_NONE, GL_NONE, GL_NONE},
      
  // -- standard HDR RGB --
  
  {GL_RGBA,         GL_NONE,         GL_RGBA32UI},
  {GL_RGBA,         GL_FLOAT,        GL_RGBA32F},
  {GL_RGBA_INTEGER, GL_UNSIGNED_INT, GL_RGBA32UI},
  {GL_RGBA_INTEGER, GL_INT,          GL_RGBA32I},
  {GL_RGB,          GL_NONE,         GL_RGB32UI},
  {GL_RGB,          GL_FLOAT,        GL_RGB32F},
  {GL_RGB_INTEGER,  GL_UNSIGNED_INT, GL_RGB32UI},
  {GL_RGB_INTEGER,  GL_INT,          GL_RGB32I},
  
  {GL_RGBA,         GL_NONE,           GL_RGBA16},
  {GL_RGBA,         GL_HALF_FLOAT,     GL_RGBA16F},
  {GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, GL_RGBA16UI},
  {GL_RGBA_INTEGER, GL_SHORT,          GL_RGBA16I},
  {GL_RGBA,         GL_UNSIGNED_SHORT, GL_RGBA16},
  {GL_RGBA,         GL_SHORT,          GL_RGBA16_SNORM},
  
  // -- standard RGB --
  
  {GL_RGBA,         GL_NONE,          GL_RGBA8},
  {GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, GL_RGBA8UI},
  {GL_RGBA_INTEGER, GL_BYTE,          GL_RGBA8I},
  {GL_RGBA,         GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8},
  {GL_RGBA,         GL_UNSIGNED_BYTE, GL_RGBA8},
  {GL_RGBA,         GL_BYTE,          GL_RGBA8_SNORM},
  
  {GL_BGRA, GL_NONE,                     GL_RGBA8},
  {GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, GL_SRGB8_ALPHA8},
  {GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, GL_RGBA8},
  
  // -- packed RGB --
  
  {GL_RGBA_INTEGER, GL_NONE,                     GL_RGB10_A2},
  {GL_RGBA_INTEGER, GL_UNSIGNED_INT_10_10_10_2,  GL_RGB10_A2},
  {GL_RGBA_INTEGER, GL_UNSIGNED_INT_10_10_10_2,  GL_RGB10_A2UI},
  {GL_RGB,          GL_R11F_G11F_B10F,           GL_R11F_G11F_B10F},
  {GL_RGB_INTEGER,  GL_UNSIGNED_INT_5_9_9_9_REV, GL_RGB9_E5},
  
  {GL_BGR,  GL_UNSIGNED_SHORT_5_6_5,       GL_RGB565},
  {GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_RGB5_A1},
  {GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_RGBA4},

  // -- components --
  
  {GL_RG,         GL_NONE,           GL_RG32UI},
  {GL_RG,         GL_FLOAT,          GL_RG32F},
  {GL_RG_INTEGER, GL_UNSIGNED_INT,   GL_RG32UI},
  {GL_RG_INTEGER, GL_INT,            GL_RG32I},
  {GL_RG,         GL_NONE,           GL_RG16},
  {GL_RG,         GL_HALF_FLOAT,     GL_RG16F},
  {GL_RG_INTEGER, GL_UNSIGNED_SHORT, GL_RG16UI},
  {GL_RG_INTEGER, GL_SHORT,          GL_RG16I},
  {GL_RG,         GL_UNSIGNED_SHORT, GL_RG16},
  {GL_RG,         GL_SHORT,          GL_RG16_SNORM},
  {GL_RG,         GL_NONE,           GL_RG8},
  {GL_RG_INTEGER, GL_UNSIGNED_BYTE,  GL_RG8UI},
  {GL_RG_INTEGER, GL_BYTE,           GL_RG8I},
  {GL_RG,         GL_UNSIGNED_BYTE,  GL_RG8},
  {GL_RG,         GL_BYTE,           GL_RG8_SNORM},
  
  {GL_RED,         GL_NONE,         GL_R32UI},
  {GL_RED,         GL_FLOAT,        GL_R32F},
  {GL_RED_INTEGER, GL_UNSIGNED_INT, GL_R32UI},
  {GL_RED_INTEGER, GL_INT,          GL_R32I},
  {GL_RED,         GL_NONE,           GL_R16},
  {GL_RED,         GL_HALF_FLOAT,     GL_R16F},
  {GL_RED_INTEGER, GL_UNSIGNED_SHORT, GL_R16UI},
  {GL_RED_INTEGER, GL_SHORT,          GL_R16I},
  {GL_RED,         GL_UNSIGNED_SHORT, GL_R16},
  {GL_RED,         GL_SHORT,          GL_R16_SNORM},
  {GL_RED,         GL_NONE,           GL_R8},
  {GL_RED_INTEGER, GL_UNSIGNED_BYTE,  GL_R8UI},
  {GL_RED_INTEGER, GL_BYTE,           GL_R8I},
  {GL_RED,         GL_UNSIGNED_BYTE,  GL_R8},
  {GL_RED,         GL_BYTE,           GL_R8_SNORM},
  {GL_ALPHA,       GL_UNSIGNED_BYTE,  GL_ALPHA8},

  {GL_NONE, GL_NONE, GL_NONE}
};
