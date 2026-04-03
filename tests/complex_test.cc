#include "math/complex.h"

#include <cmath>
#include <stdexcept>

#include "gtest/gtest.h"

namespace
{
   TEST(ComplexTest, DefaultConstructor)
   {
      math::complex c;
      EXPECT_EQ(c.real(), 0);
      EXPECT_EQ(c.imaginary(), 0);
   }

   TEST(ComplexTest, SingleConstructor)
   {
      math::complex c(1);
      EXPECT_EQ(c.real(), 1);
      EXPECT_EQ(c.imaginary(), 0);
   }

   TEST(ComplexTest, DoubleConstructor)
   {
      math::complex c(1,2);
      EXPECT_EQ(c.real(), 1);
      EXPECT_EQ(c.imaginary(), 2);
   }

   TEST(ComplexTest, CopyConstructor)
   {
      math::complex c1(1,2);
      math::complex c2=c1;
      EXPECT_EQ(c2.real(), 1);
      EXPECT_EQ(c2.imaginary(), 2);
   }

   TEST(ComplexTest, AssignmentOperator)
   {
      math::complex c1(1,2);
      math::complex c2;
      c2=c1;
      EXPECT_EQ(c2.real(), 1);
      EXPECT_EQ(c2.imaginary(), 2);
   }
   
   TEST(ComplexTest, PlusOperator)
   {
      math::complex c1(1,2);
      math::complex c2(2,3);
      math::complex c3=c1+c2;
      EXPECT_EQ(c3.real(), 3);
      EXPECT_EQ(c3.imaginary(), 5);
   }

   TEST(ComplexTest, PlusAssignmentOperator)
   {
      math::complex c1(1,2);
      math::complex c2(2,3);
      c1+=c2;
      EXPECT_EQ(c1.real(), 3);
      EXPECT_EQ(c1.imaginary(), 5);
   }

   TEST(ComplexTest, MinusOperator)
   {
      math::complex c1(1,2);
      math::complex c2(2,4);
      math::complex c3=c1-c2;
      EXPECT_EQ(c3.real(), -1);
      EXPECT_EQ(c3.imaginary(), -2);
   }

   TEST(ComplexTest, MinusAssignmentOperator)
   {
      math::complex c1(1,2);
      math::complex c2(2,4);
      c1-=c2;
      EXPECT_EQ(c1.real(), -1);
      EXPECT_EQ(c1.imaginary(), -2);
   }

   TEST(ComplexTest, TimesOperator)
   {
      math::complex c1(1,2);
      math::complex c2(2,3);
      math::complex c3=c1*c2;
      EXPECT_EQ(c3.real(), -4);
      EXPECT_EQ(c3.imaginary(), 7);
   }

   TEST(ComplexTest, TimesAssignmentOperator)
   {
      math::complex c1(1,2);
      math::complex c2(2,3);
      c1*=c2;
      EXPECT_EQ(c1.real(), -4);
      EXPECT_EQ(c1.imaginary(), 7);
   }

   TEST(ComplexTest, DivideOperator)
   {
      math::complex c1(1,2);
      math::complex c2(1,3);
      math::complex c3=c1/c2;
      EXPECT_EQ(c3.real(), 0.7);
      EXPECT_EQ(c3.imaginary(), -0.1);
   }

   TEST(ComplexTest, DivideOperatorZero)
   {
      math::complex c1(1,2);
      math::complex c2;
      EXPECT_THROW(c1/c2, std::domain_error);
   }

   TEST(ComplexTest, DivideAssignmentOperator)
   {
      math::complex c1(1,2);
      math::complex c2(1,3);
      c1/=c2;
      EXPECT_EQ(c1.real(), 0.7);
      EXPECT_EQ(c1.imaginary(), -0.1);
   }

   TEST(ComplexTest, DivideAssignmentOperatorZero)
   {
      math::complex c1(1,2);
      math::complex c2;
      EXPECT_THROW(c1/=c2, std::domain_error);
   }

   TEST(ComplexTest, EqualsOperator)
   {
      math::complex c1(1,2);
      math::complex c2(1,3);
      math::complex c3(1,3);
      EXPECT_NE(c1, c2);
      EXPECT_EQ(c2, c3);
   }

   TEST(ComplexTest, OperatorDouble)
   {
      math::complex c1(1,2);
      double x=c1;
      EXPECT_EQ(x, 1);
   }

   TEST(ComplexTest, Norm)
   {
      math::complex c(3,4);
      EXPECT_EQ(c.norm(), 5);
   }

   TEST(ComplexTest, NormSquared)
   {
      math::complex c(1,2);
      EXPECT_EQ(c.normSquared(), 5);
   }

   TEST(ComplexTest, Arg)
   {
      math::complex c(1,1);
      EXPECT_EQ(c.arg(), M_PI/4);
   }

   TEST(ComplexTest, Complement)
   {
      math::complex c1(1,3);
      math::complex c2=c1.complement();
      EXPECT_EQ(c2.real(), 1);
      EXPECT_EQ(c2.imaginary(), -3);
   }
}
