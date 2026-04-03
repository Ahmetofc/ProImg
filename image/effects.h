#ifndef DSP_IMAGE_EFFECTS_H_
#define DSP_IMAGE_EFFECTS_H_

#include <cmath>
#include <fstream>
#include <functional>
#include <string>

#include "image/ppm.h"
#include "math/convolution.h"
#include "math/matrix.h"
#include "math/vector.h"

namespace image
{

   inline math::matrix<double> extractR(const math::matrix<double>&) noexcept;
   inline math::matrix<double> extractG(const math::matrix<double>&) noexcept;
   inline math::matrix<double> extractB(const math::matrix<double>&) noexcept;

   inline math::matrix<double> extractY(const math::matrix<double>&) noexcept;
   inline math::matrix<double> extractCb(const math::matrix<double>&) noexcept;
   inline math::matrix<double> extractCr(const math::matrix<double>&) noexcept;

   inline math::matrix<double> RGBToYCbCr(const math::matrix<double>&);
   inline math::matrix<double> RGBToYCbCr(const math::matrix<double>&, const math::matrix<double>&, const math::matrix<double>&);
   inline math::matrix<double> YCbCrToRGB(const math::matrix<double>&);
   inline math::matrix<double> YCbCrToRGB(const math::matrix<double>&, const math::matrix<double>&, const math::matrix<double>&);

   //maybe add opengl to this in the future
   inline void visualize(const math::matrix<double>&, std::string, u16, u16);

   inline math::matrix<double> exposure(const math::matrix<double>&, double) noexcept;
   inline math::matrix<double> exposure(const math::matrix<double>&, double, double, double);

   inline math::matrix<double> saturation(const math::matrix<double>&, double);
   inline math::matrix<double> saturation(const math::matrix<double>&, double, double);

   inline math::matrix<double> contrast(const math::matrix<double>&, double);

   inline math::matrix<double> gaussianBlur(const math::matrix<double>&, double, u32, u32);

   //add  vibrancy - noise - glow - gamma - sharpen - hue
}

#include "image/effects-inl.h"

#endif
