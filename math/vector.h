#ifndef PROIMG_MATH_VECTOR_H_
#define PROIMG_MATH_VECTOR_H_

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "math/complex.h"
#include "math/types.h"

namespace math
{
   template <typename T>
   class vector
   {
      public:
         constexpr vector() noexcept=default;
         constexpr explicit vector(u32) noexcept;
         constexpr vector(const std::initializer_list<T>&) noexcept;
         constexpr vector(const std::vector<T>&) noexcept;
         template <typename R>
         constexpr vector(const std::vector<R>&);
         constexpr vector(const vector&) noexcept;
         ~vector() noexcept=default;

         vector& operator=(const vector&) noexcept;

         vector operator+(const vector&) const;
         vector& operator+=(const vector&);
         vector operator-(const vector&) const;
         vector& operator-=(const vector&);
         complex operator*(const vector&) const;
         vector operator/(const vector&) const;
         vector& operator/=(const vector&);

         bool operator==(const vector&) const noexcept;

         operator std::vector<T>() const noexcept;

         constexpr u32 dimension() const noexcept;
         constexpr std::vector<T> getData() const noexcept;
         constexpr void writeData(u32, T);

         constexpr double magnitude() const noexcept;
         constexpr double magnitudeSquared() const noexcept;

         vector multiplyElements(const vector&) const;
         vector& multiplyElementsAssign(const vector&);
         vector crossProduct(const vector&) const;
         vector& crossProductAssign(const vector&);

         friend math::vector<T> operator*(const math::vector<T>& lhs, math::complex rhs) noexcept
         {
            vector res=lhs;
            for(T& element : res.data)
               element*=rhs;
            return res;
         }
         friend math::vector<T> operator*(complex lhs, const math::vector<T>& rhs) noexcept
         {
            vector res=rhs;
            for(T& element : res.data)
               element*=lhs;
            return res;
         }
         friend math::vector<T>& operator*=(math::vector<T>& lhs, math::complex rhs) noexcept
         {
            for(T& element : lhs.data)
               element*=rhs;
            return lhs;
         }
         friend math::vector<T> operator/(const math::vector<T>& lhs, math::complex rhs)
         {
            if(rhs==complex{})
               throw std::domain_error("Division by zero.\n");
            vector res=lhs;
            for(T& element : res.data)
               element/=rhs;
            return res;
         }
         friend math::vector<T>& operator/=(math::vector<T>& lhs, math::complex rhs)
         {
            if(rhs==complex{})
               throw std::domain_error("Division by zero.\n");
            for(T& element : lhs.data)
               element/=rhs;
            return lhs;
         }
         friend std::ostream& operator<<(std::ostream& output, const math::vector<T>& input) noexcept
         {
            output << "Vector with dimension " << input.dim << std::endl;
            output << "Vector entries:" << std::endl;
            for(const T& element : input.data)
               output << element << " ";
            output << std::endl;
            return output;
         }

      private:
         u32 dim{};
         std::vector<T> data{};
   };
}

#include "math/vector-inl.h"

#endif
