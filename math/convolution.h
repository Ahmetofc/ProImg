#ifndef PROIMG_MATH_CONVOLUTION_H_
#define PROIMG_MATH_CONVOLUTION_H_

#include "math/matrix.h"
#include "math/vector.h"

namespace math
{
   inline matrix<double> convolve1D(const matrix<double>&, const matrix<double>&) noexcept;
   inline matrix<double> convolve2D(const matrix<double>&, const matrix<double>&, const matrix<double>&) noexcept;
   inline matrix<double> convolve2D(const matrix<double>&, const matrix<double>&) noexcept;
   
   inline matrix<double> convolve1DNormalized(const matrix<double>&, const matrix<double>&) noexcept;
   inline matrix<double> convolve2DNormalized(const matrix<double>&, const matrix<double>&) noexcept;
   
   template <typename T>
   inline matrix<T> get2DMask(const vector<T>&, const vector<T>&);
}

#include "math/convolution-inl.h"

#endif
