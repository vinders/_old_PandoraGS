/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : mathematics toolset
*******************************************************************************/
#pragma once

#ifndef _MATH__USE_ASSEMBLY
#define _MATH__USE_ASSEMBLY  1
#endif
#ifndef _MATH__USE_SSE
#define _MATH__USE_SSE  1
#endif

#include <cstdint>
#include <cstddef>
#include <cmath>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace algorithm
    /// Algorithms
    namespace algorithm
    {
        /// @class Math
        /// @brief Mathematics toolset
        class Math
        {
        public:
            /// @brief Get greatest common divider calculation (Euclide)
            template <typename T>
            static T gcd(T lhs, T rhs) noexcept
            {
                if (lhs == 0) 
                    return rhs;

                T remainder;
                while (rhs != 0) // if (rhs == 0) return lhs;
                {
                    remainder = lhs % rhs;
                    lhs = rhs;
                    rhs = remainder;
                }
                return lhs;
            }
        
            /// @brief Square-root calculation (100% precision)
            static inline double sqrt(double val)
            {
                return std::sqrt(val);
            }
            /// @brief Fast square-root calculation (99.9% precision)
#           if _MATH__USE_ASSEMBLY
            static inline float __declspec(naked) __fastcall sqrtFast(float val)
            {
                static_assert(sizeof(float) == 4);
                _asm fld qword ptr [esp+4] // load 'val' into FPU register
                _asm fsqrt
                _asm ret 4 // pop 'val' from stack
            }
#           else
            static inline float sqrtFast(float val)
            {
                // J.Carmack (Quake III Arena) - Newton's method approximation
                static_assert(sizeof(float) == sizeof(int32_t));
                float x2 = val * 0.5f;
                union
                {
                    float y;
                    int32_t i;
                } op;
                op.y = val;
                
                op.i = 0x5f3759df - (op.i >> 1); // initial guess y0
                op.y = op.y * (1.5f - (x2 * op.y * op.y));   // 1st iteration
                //op.y = op.y * (1.5f - (x2 * op.y * op.y)); // 2nd iteration, this can be removed
                return op.y * val;
            } 
#           endif
        
            /// @brief Inverse square-root calculation (100% precision)
            static inline double invSqrt(double val)
            {
                return (1.0 / std::sqrt(val));
            }

            /// @brief Fast inverse square-root calculation (99.9% precision)
            static inline float invSqrtFast(float val)
            {
                #if _MATH__USE_ASSEMBLY and _MATH__USE_SSE
                _asm rsqrtss xmm0, val
                __asm movss val, xmm0
                return val;
                
                #else
                // J.Carmack (Quake III Arena) - Newton's method approximation
                static_assert(sizeof(float) == sizeof(int32_t));
                float x2 = val * 0.5f;
                union
                {
                    float y;
                    int32_t i;
                } op;
                op.y = val;
                
                op.i = 0x5f3759df - (op.i >> 1); // initial guess y0
                op.y = op.y * (1.5f - (x2 * op.y * op.y));   // 1st iteration
                //op.y = op.y * (1.5f - (x2 * op.y * op.y)); // 2nd iteration, this can be removed
                return op.y;
                #endif
            }
        };
    }
}
