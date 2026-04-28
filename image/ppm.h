#ifndef PROIMG_IMAGE_PPM_H
#define PROIMG_IMAGE_PPM_H

#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

#include "math/matrix.h"
#include "math/types.h"

namespace image
{
   struct ppmArgs
   {
      public:
         std::string type="P6";
         u16 width=1024, height=1024, maxVal=255;
   };

   inline double clamp(double);
   inline double gammaCorrection(u8);
   inline u8 inverseGammaCorrection(double);

    inline std::string ppmGetline(std::ifstream&);

   inline void ppmHeader(std::ofstream&, const ppmArgs={});
   inline ppmArgs ppmHeaderInfo(std::ifstream&);
   inline void ppmData(std::ofstream&, const math::matrix<double>&, const ppmArgs={});
   inline math::matrix<double> ppmExtractData(std::ifstream&, const ppmArgs={});
}

#include "image/ppm-inl.h"

#endif
