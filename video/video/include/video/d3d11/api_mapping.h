/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#pragma once
#ifdef _WINDOWS

#include <cassert>
#include <dxgi.h>
#include <d3d11.h>
#include "video/abstract_enums.h"

namespace pandora {
  namespace video {
    namespace d3d11 {
      /// @class ApiMapping
      /// @brief Abstract rendering information to Direct3D11 rendering information converter
      class ApiMapping final {
      public:
        ApiMapping() = delete;
        
        // -- drawing --
        
        /// @brief Convert face culling mode
        static D3D11_CULL_MODE toCullingMode(CullingMode mode) noexcept;
        /// @brief Convert texture dimension type
        static D3D_SRV_DIMENSION toTextureDimension(TextureDimension type) noexcept;
        /// @brief Convert texture addressing/repeating mode
        static D3D11_TEXTURE_ADDRESS_MODE toTextureAddressMode(TextureAddressMode mode) noexcept;
        
        // -- operations --
        
        /// @brief Convert stencil operation type
        static D3D11_STENCIL_OP toStencilOperation(StencilOperation mode) noexcept;
        /// @brief Convert blending operation type
        static D3D11_BLEND_OP toBlendOperation(BlendOperation op) noexcept;
        /// @brief Convert factor type for blending operations
        static D3D11_BLEND toBlendFactor(BlendFactor mode) noexcept;
        
        // -- colors --

        /// @brief Convert pixel color format
        static inline DXGI_FORMAT toPixelFormat(pandora::video::PixelFormat format) noexcept {
          assert((uint32_t)format <= (uint32_t)pandora::video::PixelFormat::end);
          return _formats[(uint32_t)format];
        }
        
      private:
        static DXGI_FORMAT _formats[(uint32_t)pandora::video::PixelFormat::end + 1u];
      };
    }
  }
}
#endif
