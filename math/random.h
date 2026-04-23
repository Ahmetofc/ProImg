#ifndef PROIMG_MATH_RANDOM_H_
#define PROIMG_MATH_RANDOM_H_

#include <random>

#include "math/macros.h"
#include "math/types.h"

namespace math
{
   std::vector<u16> getRandomVector(u16 size, u16 range=RANDRANGE) noexcept
   {
      std::vector<u16> res(size);
      std::random_device dev;
      std::mt19937 generator(dev());
      std::uniform_int_distribution<> pdf(0, range);
      for(u16& value : res)
         value=pdf(generator);
      return res;
   }
   std::vector<std::vector<u16>> getRandomMatrix(u16 size, u16 range=RANDRANGE) noexcept
   {
      std::vector<std::vector<u16>> res(size);
      for(std::vector<u16>& vec : res)
         vec=getRandomVector(size, range);
      return res;
   }
}

#endif
