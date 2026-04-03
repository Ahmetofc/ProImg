#include "math/vector.h"

#include <cmath>
#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"

#include "math/complex.h"

namespace
{
   TEST(VectorTest, DefaultConstructor)
   {
      math::vector<double> v;
      EXPECT_EQ(v.dimension(), 0);
      EXPECT_EQ(v.getData().size(), 0);
   }

   TEST(VectorTest, SingleConstructor)
   {
      math::vector<double> v(5);
      EXPECT_EQ(v.dimension(), 5);
      EXPECT_EQ(v.getData().size(), 5);
      EXPECT_EQ(v.getData().at(0), 0);
   }

   TEST(VectorTest, InitializerListConstructor)
   {
      math::vector<double> v={1,2,3};
      EXPECT_EQ(v.dimension(), 3);
      EXPECT_EQ(v.getData().size(), 3);
      EXPECT_EQ(v.getData().at(0), 1);
      EXPECT_EQ(v.getData().at(1), 2);
      EXPECT_EQ(v.getData().at(2), 3);
   }

   TEST(VectorTest, VectorConstructor)
   {
      std::vector<double> v1={1,2,3};
      math::vector<double> v2=v1;
      EXPECT_EQ(v2.dimension(), 3);
      EXPECT_EQ(v2.getData().size(), 3);
      EXPECT_EQ(v2.getData().at(0), 1);
      EXPECT_EQ(v2.getData().at(1), 2);
      EXPECT_EQ(v2.getData().at(2), 3);
   }

   TEST(VectorTest, CopyConstructor)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2=v1;
      EXPECT_EQ(v2.dimension(), 3);
      EXPECT_EQ(v2.getData().size(), 3);
      EXPECT_EQ(v2.getData().at(0), 1);
      EXPECT_EQ(v2.getData().at(1), 2);
      EXPECT_EQ(v2.getData().at(2), 3);
   }

   TEST(VectorTest, AssignmentOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2;
      v2=v1;
      EXPECT_EQ(v2.dimension(), 3);
      EXPECT_EQ(v2.getData().size(), 3);
      EXPECT_EQ(v2.getData().at(0), 1);
      EXPECT_EQ(v2.getData().at(1), 2);
      EXPECT_EQ(v2.getData().at(2), 3);
   }

   TEST(VectorTest, PlusOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,4};
      math::vector<double> v3=v1+v2;
      EXPECT_EQ(v3.dimension(), 3);
      EXPECT_EQ(v3.getData().size(), 3);
      EXPECT_EQ(v3.getData().at(0), 3);
      EXPECT_EQ(v3.getData().at(1), 5);
      EXPECT_EQ(v3.getData().at(2), 7);
   }
   
   TEST(VectorTest, PlusOperatorSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3};
      EXPECT_THROW(v1+v2, std::length_error);
   }

   TEST(VectorTest, PlusAssignmentOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,4};
      v1+=v2;
      EXPECT_EQ(v1.dimension(), 3);
      EXPECT_EQ(v1.getData().size(), 3);
      EXPECT_EQ(v1.getData().at(0), 3);
      EXPECT_EQ(v1.getData().at(1), 5);
      EXPECT_EQ(v1.getData().at(2), 7);
   }

   TEST(VectorTest, PlusAssignmentOperatorSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3};
      EXPECT_THROW(v1+=v2, std::length_error);
   }

   TEST(VectorTest, MinusOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,4};
      math::vector<double> v3=v1-v2;
      EXPECT_EQ(v3.dimension(), 3);
      EXPECT_EQ(v3.getData().size(), 3);
      EXPECT_EQ(v3.getData().at(0), -1);
      EXPECT_EQ(v3.getData().at(1), -1);
      EXPECT_EQ(v3.getData().at(2), -1);
   }
   
   TEST(VectorTest, MinusOperatorSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3};
      EXPECT_THROW(v1-v2, std::length_error);
   }

   TEST(VectorTest, MinusAssignmentOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,4};
      v1-=v2;
      EXPECT_EQ(v1.dimension(), 3);
      EXPECT_EQ(v1.getData().size(), 3);
      EXPECT_EQ(v1.getData().at(0), -1);
      EXPECT_EQ(v1.getData().at(1), -1);
      EXPECT_EQ(v1.getData().at(2), -1);
   }

   TEST(VectorTest, MinusAssignmentOperatorSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3};
      EXPECT_THROW(v1-=v2, std::length_error);
   }

   TEST(VectorTest, TimesOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,4};
      double c=v1*v2;
      EXPECT_EQ(c, 20);
   }
   
   TEST(VectorTest, TimesOperatorSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3};
      EXPECT_THROW(v1*v2, std::length_error);
   }

   TEST(VectorTest, DivideOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,4,5};
      math::vector<double> v3=v1/v2;
      EXPECT_EQ(v3.dimension(), 3);
      EXPECT_EQ(v3.getData().size(), 3);
      EXPECT_EQ(v3.getData().at(0), 0.5);
      EXPECT_EQ(v3.getData().at(1), 0.5);
      EXPECT_EQ(v3.getData().at(2), 0.6);
   }
   
   TEST(VectorTest, DivideOperatorSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3};
      EXPECT_THROW(v1/v2, std::length_error);
   }

   TEST(VectorTest, DivideOperatorZero)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,0};
      EXPECT_THROW(v1/v2, std::domain_error);
   }

   TEST(VectorTest, DivideAssignmentOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,4,5};
      v1/=v2;
      EXPECT_EQ(v1.dimension(), 3);
      EXPECT_EQ(v1.getData().size(), 3);
      EXPECT_EQ(v1.getData().at(0), 0.5);
      EXPECT_EQ(v1.getData().at(1), 0.5);
      EXPECT_EQ(v1.getData().at(2), 0.6);
   }

   TEST(VectorTest, DivideAssignmentOperatorSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3};
      EXPECT_THROW(v1/=v2, std::length_error);
   }

   TEST(VectorTest, DivideAssignmentOperatorZero)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,0};
      EXPECT_THROW(v1/=v2, std::domain_error);
   }

   TEST(VectorTest, EqualsOperator)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={2,3,0};
      math::vector<double> v3={2,3,0};
      EXPECT_NE(v1, v2);
      EXPECT_EQ(v2, v3);
   }

   TEST(VectorTest, WriteData)
   {
      math::vector<double> v={1,2,3};
      v.writeData(0, 3);
      EXPECT_EQ(v, std::vector<double>({3,2,3}));
   }

   TEST(VectorTest, WriteDataIndex)
   {
      math::vector<double> v={1,2,3};
      EXPECT_THROW(v.writeData(3, 4), std::range_error);
   }

   TEST(VectorTest, OperatorVector)
   {
      math::vector<double> v1={1,2,3};
      std::vector<double> v2=v1;
      EXPECT_EQ(v1.dimension(), v2.size());
      EXPECT_EQ(v1.getData().at(0), v2.at(0));
      EXPECT_EQ(v1.getData().at(1), v2.at(1));
      EXPECT_EQ(v1.getData().at(2), v2.at(2));
   }

   TEST(VectorTest, Magnitude)
   {
      math::vector<double> v1={1,2,3};
      math::vector<math::complex> v2={math::complex{1,1}, math::complex{2,-3}};
      EXPECT_EQ(v1.magnitude(), std::sqrt(14));
      EXPECT_EQ(v2.magnitude(), std::sqrt(15));
   }

   TEST(VectorTest, MagnitudeSquared)
   {
      math::vector<double> v1={3,2,5};
      math::vector<math::complex> v2={math::complex{1,4}, math::complex{2,-7}};
      EXPECT_EQ(v1.magnitudeSquared(), 38);
      EXPECT_EQ(v2.magnitudeSquared(), 70);
   }

   TEST(VectorTest, MultiplyElements)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3,4};
      math::vector<double> v3=v1.multiplyElements(v2);
      EXPECT_EQ(v3.dimension(), 3);
      EXPECT_EQ(v3.getData().at(0), 0);
      EXPECT_EQ(v3.getData().at(1), 6);
      EXPECT_EQ(v3.getData().at(2), 12);
   }

   TEST(VectorTest, MultiplyElementsSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3};
      EXPECT_THROW(v1.multiplyElements(v2), std::length_error);
   }

   TEST(VectorTest, MultiplyElementsAssign)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3,4};
      v1.multiplyElementsAssign(v2);
      EXPECT_EQ(v1.dimension(), 3);
      EXPECT_EQ(v1.getData().at(0), 0);
      EXPECT_EQ(v1.getData().at(1), 6);
      EXPECT_EQ(v1.getData().at(2), 12);
   }

   TEST(VectorTest, MultiplyElementsAssignSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3};
      EXPECT_THROW(v1.multiplyElementsAssign(v2), std::length_error);
   }

   TEST(VectorTest, CrossProduct)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3,4};
      math::vector<double> v3=v1.crossProduct(v2);
      EXPECT_EQ(v3.dimension(), 3);
      EXPECT_EQ(v3.getData().at(0), -1);
      EXPECT_EQ(v3.getData().at(1), -4);
      EXPECT_EQ(v3.getData().at(2), 3);
   }

   TEST(VectorTest, CrossProductSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3,4,5};
      EXPECT_THROW(v1.crossProduct(v2), std::length_error);
   }

   TEST(VectorTest, CrossProductDimension)
   {
      math::vector<double> v1={1,2};
      math::vector<double> v2={0,3};
      EXPECT_THROW(v1.crossProduct(v2), std::domain_error);
   }

   TEST(VectorTest, CrossProductAssign)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3,4};
      v1.crossProductAssign(v2);
      EXPECT_EQ(v1.dimension(), 3);
      EXPECT_EQ(v1.getData().at(0), -1);
      EXPECT_EQ(v1.getData().at(1), -4);
      EXPECT_EQ(v1.getData().at(2), 3);
   }

   TEST(VectorTest, CrossProductAssignSize)
   {
      math::vector<double> v1={1,2,3};
      math::vector<double> v2={0,3,4,5};
      EXPECT_THROW(v1.crossProductAssign(v2), std::length_error);
   }

   TEST(VectorTest, CrossProductAssignDimension)
   {
      math::vector<double> v1={1,2};
      math::vector<double> v2={0,3};
      EXPECT_THROW(v1.crossProductAssign(v2), std::domain_error);
   }

   TEST(VectorTest, FriendTimesOperatorLeft)
   {
      math::vector<double> v1={1,2,3};
      double factor=2;
      math::vector<double> v2=v1*factor;
      EXPECT_EQ(v2.dimension(), 3);
      EXPECT_EQ(v2.getData().at(0), 2);
      EXPECT_EQ(v2.getData().at(1), 4);
      EXPECT_EQ(v2.getData().at(2), 6);
   }

   TEST(VectorTest, FriendTimesOperatorRight)
   {
      math::vector<double> v1={1,2,3};
      double factor=2;
      math::vector<double> v2=factor*v1;
      EXPECT_EQ(v2.dimension(), 3);
      EXPECT_EQ(v2.getData().at(0), 2);
      EXPECT_EQ(v2.getData().at(1), 4);
      EXPECT_EQ(v2.getData().at(2), 6);
   }

   TEST(VectorTest, FriendTimesAssignmentOperator)
   {
      math::vector<double> v={1,2,3};
      double factor=2;
      v*=factor;
      EXPECT_EQ(v.dimension(), 3);
      EXPECT_EQ(v.getData().at(0), 2);
      EXPECT_EQ(v.getData().at(1), 4);
      EXPECT_EQ(v.getData().at(2), 6);
   }

   TEST(VectorTest, FriendDivideOperator)
   {
      math::vector<double> v1={1,2,3};
      double factor=2;
      math::vector<double> v2=v1/factor;
      EXPECT_EQ(v2.dimension(), 3);
      EXPECT_EQ(v2.getData().at(0), 0.5);
      EXPECT_EQ(v2.getData().at(1), 1);
      EXPECT_EQ(v2.getData().at(2), 1.5);
   }

   TEST(VectorTest, FriendDivideOperatorZero)
   {
      math::vector<double> v={1,2,3};
      double factor=0;
      EXPECT_THROW(v/factor, std::domain_error);
   }

   TEST(VectorTest, FriendDivideAssignmentOperator)
   {
      math::vector<double> v={1,2,3};
      double factor=2;
      v/=factor;
      EXPECT_EQ(v.dimension(), 3);
      EXPECT_EQ(v.getData().at(0), 0.5);
      EXPECT_EQ(v.getData().at(1), 1);
      EXPECT_EQ(v.getData().at(2), 1.5);
   }

   TEST(VectorTest, FriendDivideAssignmentOperatorZero)
   {
      math::vector<double> v={1,2,3};
      double factor=0;
      EXPECT_THROW(v/=factor, std::domain_error);
   }
}
