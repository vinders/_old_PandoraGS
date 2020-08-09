/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace pandora { 
  namespace video { 
    /// @class GraphicsApi
    /// @brief Interface for standard bindings of a graphics API
    class GraphicsApi {
    public:
      GraphicsApi() = delete;
    };
    
    /*template<typename _T>
    concept GraphicsApi = requires {
      typename _T::innerType;
    } 
    && requires {
      { _T::staticFunction(int{}) } -> bool;
    }
    && requires(_T api) {
      { api.~_T() } noexcept;
      { api.method(int{}) } -> bool;
    };*/
  }
}
