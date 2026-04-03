#include "math/range.h"

#include <stdexcept>

#include "gtest/gtest.h"

namespace
{
   TEST(RangeTest, SingleConstructor)
   {
      math::intRange r(2);
      EXPECT_EQ(r.start, 2);
      EXPECT_EQ(r.end, 2);
   }

   TEST(RangeTest, DoubleConstructor)
   {
      math::intRange r(2,3);
      EXPECT_EQ(r.start, 2);
      EXPECT_EQ(r.end, 3);
   }

   TEST(RangeTest, DoubleConstructorRange)
   {
      EXPECT_THROW(math::intRange(3,2), std::range_error);
   }

   TEST(RangeTest, InitializerListConstructor)
   {
      math::intRange r={2,3};
      EXPECT_EQ(r.start, 2);
      EXPECT_EQ(r.end, 3);
   }

   TEST(RangeTest, InitializerListConstructorRange)
   {
      EXPECT_THROW(math::intRange({3,2}), std::range_error);
   }

   TEST(RangeTest, Span)
   {
      math::intRange r={1,6};
      EXPECT_EQ(r.span(), 6);
   }
}
