#ifndef DSP_MATH_RANGE_H_
#define DSP_MATH_RANGE_H_

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

#include "math/types.h"

namespace math
{
   class intRange
   {
      public:
         intRange()=delete;
         constexpr intRange(u32 inst) noexcept : start(inst), end(inst)
         {
         }
         constexpr intRange(u32 _start, u32 _end) : start(_start), end(_end)
         {
            if(start>end)
               throw std::range_error("Invalid range.\n");
         }
         constexpr intRange(const std::initializer_list<u32>& _data) : start(std::data(_data)[0]), end(std::data(_data)[1])
         {
            if(start>end)
               throw std::range_error("Invalid range.\n");
         }
         ~intRange() noexcept=default;

         constexpr u32 span() noexcept
         {
            return end-start+1;
         }

      public:
         const u32 start, end;
   };
}

#endif
