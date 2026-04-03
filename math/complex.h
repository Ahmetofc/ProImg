#ifndef DSP_MATH_COMPLEX_H_
#define DSP_MATH_COMPLEX_H_

#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "math/macros.h"
#include "math/types.h"

namespace math
{
   class complex
   {
      public:
         constexpr complex() noexcept=default;
         constexpr complex(double) noexcept;
         constexpr complex(double, double) noexcept;
         constexpr complex(const complex&) noexcept;
         ~complex() noexcept=default;

         complex& operator=(const complex&) noexcept;

         complex operator+(const complex&) const noexcept;
         complex& operator+=(const complex&) noexcept;
         complex operator-(const complex&) const noexcept;
         complex& operator-=(const complex&) noexcept;
         complex operator*(const complex&) const noexcept;
         complex& operator*=(const complex&) noexcept;
         complex operator/(const complex&) const;
         complex& operator/=(const complex&);

         bool operator==(const complex&) const noexcept;

         operator double() const noexcept;

         constexpr double real() const noexcept;
         constexpr double imaginary() const noexcept;

         double norm() const noexcept;
         double normSquared() const noexcept;
         double arg() const noexcept;

         complex complement() const noexcept;

         friend std::ostream& operator<<(std::ostream& output, const complex& other) noexcept
         {
            output << std::fixed << std::setprecision(FLOATPRECISION);
            if(other.im<0)
               output << other.re << other.im << "j";
            else if(other.im>0)
               output << other.re << "+" << other.im << "j";
            else 
               output << other.re;
            return output;
         }  

      private:
         double re{}, im{};
   };
}

#include "math/complex-inl.h"

#endif
