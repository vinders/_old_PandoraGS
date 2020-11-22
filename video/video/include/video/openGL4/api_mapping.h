/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#pragma once

#include <cassert>
#include "video/api/opengl.h"
#include "video/abstract_enums.h"

namespace pandora {
  namespace video {
    namespace openGL4 {
      /// @brief OpenGL pixel format type
      struct GLPixelFormat {
        GLenum components; ///< pixel components/order (GL_RGBA, GL_BGRA, GL_RG, GL_RED...)
        GLenum type;       ///< type of components (GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_FLOAT...)
        GLenum format;     ///< format identifier (ex: GL_RGBA8UI)
      };
      
      // ---
      
      /// @class ApiMapping
      /// @brief Abstract rendering information to OpenGL rendering information converter
      class ApiMapping final {
      public:
        ApiMapping() = delete;
        
        // -- drawing --
        
        /// @brief Convert face culling mode
        static GLenum toCullingMode(CullingMode mode) noexcept;
        /// @brief Convert texture dimension type
        static GLenum toTextureDimension(TextureDimension type) noexcept;
        /// @brief Convert texture addressing/repeating mode
        static GLenum toTextureAddressMode(TextureAddressMode mode) noexcept;
        
        // -- operations --
        
        /// @brief Convert stencil operation type
        static GLenum toStencilOperation(StencilOperation mode) noexcept;
        /// @brief Convert blending operation type
        static GLenum toBlendOperation(BlendOperation op) noexcept;
        /// @brief Convert factor type for blending operations
        static GLenum toBlendFactor(BlendFactor mode) noexcept;
        
        // -- colors --

        /// @brief Convert pixel color format
        static inline const GLPixelFormat& toPixelFormat(pandora::video::PixelFormat format) noexcept {
          assert((uint32_t)format <= (uint32_t)pandora::video::PixelFormat::end);
          return _formats[(uint32_t)format];
        }
        
      private:
        static GLPixelFormat _formats[(uint32_t)pandora::video::PixelFormat::end + 1u];
      };
    }
  }
}
