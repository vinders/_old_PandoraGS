/*******************************************************************************
Author  :     Romain Vinders
License :     MIT
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <set>
#include "video/abstract_enums.h"

namespace pandora {
  namespace video {
    namespace openGL4 {
      class RenderTarget;

      /** Depth/stencil buffer.
      Depth Buffers can be attached to render targets. Note we handle Depth & Stencil together.
      DepthBuffer sharing is handled automatically for you. However, there are times where you want
      to specifically control depth buffers to achieve certain effects or increase performance.
      You can control this by hinting Ogre with POOL IDs. Created depth buffers can live in different
      pools, or all together in the same one.
      Usually, a depth buffer can only be attached to a RenderTarget if it's dimensions are >=
      and have the same bit depth and same multisample settings. Depth Buffers are created automatically
      for new RTs when needed, and stored in the pool where the RenderTarget should have drawn from.
      By default, all RTs have the Id POOL_DEFAULT, which means all depth buffers are stored by default
      in that pool. By choosing a different Pool Id for a specific RenderTarget, that RT will only
      retrieve depth buffers from _that_ pool, therefore not conflicting with sharing depth buffers
      with other RTs (such as shadows maps).
      Setting an RT to POOL_MANUAL_USAGE means Ogre won't manage the DepthBuffer for you (not recommended)
      RTs with POOL_NO_DEPTH are very useful when you don't want to create a DepthBuffer for it. You can
      still manually attach a depth buffer though as internally POOL_NO_DEPTH & POOL_MANUAL_USAGE are
      handled in the same way.

      Width & height must be equal or higher than the render target's
      */
      class DepthBuffer final {
      public:
        DepthBuffer() = default;
        DepthBuffer(pandora::video::PixelFormat format, uint32_t width, uint32_t height);
        ~DepthBuffer() noexcept {
          for (auto it : this->_attachedRenderTargets)
            detach(*it);
        }

        DepthBuffer(const DepthBuffer&) = delete;
        DepthBuffer(DepthBuffer&&) = default;
        DepthBuffer& operator=(const DepthBuffer&) = delete;
        DepthBuffer& operator=(DepthBuffer&& rhs) = delete;

        inline pandora::video::PixelFormat pixelFormat() const noexcept { return this->_format; }
        inline uint32_t width() const noexcept { return this->_width; }
        inline uint32_t height() const noexcept { return this->_height; }

        bool isCompatible(RenderTarget& target) noexcept;
        bool attach(RenderTarget& target) noexcept;
        bool detach(RenderTarget& target) noexcept;

      private:
        std::set<RenderTarget*> _attachedRenderTargets;
        pandora::video::PixelFormat _format = pandora::video::PixelFormat::unknown;
        uint32_t _width = 0u;
        uint32_t _height = 0u;
      };
    }
  }
}
