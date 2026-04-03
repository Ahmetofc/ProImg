#include "math/matrix.h"

#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"

namespace
{
   TEST(MatrixTest, DefaultConstructor)
   {
      math::matrix<double> m;
      EXPECT_EQ(m.rowNumber(), 0);
      EXPECT_EQ(m.columnNumber(), 0);
      EXPECT_EQ(m.getData().size(), 0);
   }

   TEST(MatrixTest, DoubleConstructor)
   {
      math::matrix<double> m(2,3);
      EXPECT_EQ(m.rowNumber(), 2);
      EXPECT_EQ(m.columnNumber(), 3);
      EXPECT_EQ(m.getData().at(0), std::vector<double>(2, 0));
      EXPECT_EQ(m.getData().at(1), std::vector<double>(2, 0));
      EXPECT_EQ(m.getData().at(2), std::vector<double>(2, 0));
   }

   TEST(MatrixTest, InitializerListConstructor)
   {
      math::matrix<double> m={{2,3,4}, {1,2,3}};
      EXPECT_EQ(m.rowNumber(), 3);
      EXPECT_EQ(m.columnNumber(), 2);
      EXPECT_EQ(m.getData().at(0), std::vector<double>({2,3,4}));
      EXPECT_EQ(m.getData().at(1), std::vector<double>({1,2,3}));
   }

   TEST(MatrixTest, VectorConstructor)
   {
      std::vector<std::vector<double>> v={{1,2,3}, {2,3,4}};
      math::matrix<double> m=v;
      EXPECT_EQ(m.rowNumber(), 3);
      EXPECT_EQ(m.columnNumber(), 2);
      EXPECT_EQ(m.getData().at(0), std::vector<double>({1,2,3}));
      EXPECT_EQ(m.getData().at(1), std::vector<double>({2,3,4}));
   }

   TEST(MatrixTest, CopyConstructor)
   {
      math::matrix<double> m1={{2,3,4}, {1,2,3}};
      math::matrix<double> m2=m1;
      EXPECT_EQ(m2.rowNumber(), 3);
      EXPECT_EQ(m2.columnNumber(), 2);
      EXPECT_EQ(m2.getData().at(0), std::vector<double>({2,3,4}));
      EXPECT_EQ(m2.getData().at(1), std::vector<double>({1,2,3}));
   }

   TEST(MatrixTest, AssignmentOperator)
   {
      math::matrix<double> m1={{2,3,4}, {1,2,3}};
      math::matrix<double> m2;
      m2=m1;
      EXPECT_EQ(m2.rowNumber(), 3);
      EXPECT_EQ(m2.columnNumber(), 2);
      EXPECT_EQ(m2.getData().at(0), std::vector<double>({2,3,4}));
      EXPECT_EQ(m2.getData().at(1), std::vector<double>({1,2,3}));
   }

   TEST(MatrixTest, PlusOperator)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}};
      math::matrix<double> m3=m1+m2;
      EXPECT_EQ(m3.rowNumber(), 1);
      EXPECT_EQ(m3.columnNumber(), 2);
      EXPECT_EQ(m3.getData().at(0), std::vector<double>({4}));
      EXPECT_EQ(m3.getData().at(1), std::vector<double>({4}));
   }

   TEST(MatrixTest, PlusOperatorRowSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2,1}, {3,2}};
      EXPECT_THROW(m1+m2, std::length_error);
   }

   TEST(MatrixTest, PlusOperatorColumnSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}, {4}};
      EXPECT_THROW(m1+m2, std::length_error);
   }

   TEST(MatrixTest, PlusAssignmentOperator)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}};
      m1+=m2;
      EXPECT_EQ(m1.rowNumber(), 1);
      EXPECT_EQ(m1.columnNumber(), 2);
      EXPECT_EQ(m1.getData().at(0), std::vector<double>({4}));
      EXPECT_EQ(m1.getData().at(1), std::vector<double>({4}));
   }

   TEST(MatrixTest, PlusAssignmentOperatorRowSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2,1}, {3,2}};
      EXPECT_THROW(m1+=m2, std::length_error);
   }

   TEST(MatrixTest, PlusAssignmentOperatorColumnSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}, {4}};
      EXPECT_THROW(m1+=m2, std::length_error);
   }

   TEST(MatrixTest, MinusOperator)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}};
      math::matrix<double> m3=m1-m2;
      EXPECT_EQ(m3.rowNumber(), 1);
      EXPECT_EQ(m3.columnNumber(), 2);
      EXPECT_EQ(m3.getData().at(0), std::vector<double>({0}));
      EXPECT_EQ(m3.getData().at(1), std::vector<double>({-2}));
   }

   TEST(MatrixTest, MinusOperatorRowSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2,1}, {3,2}};
      EXPECT_THROW(m1-m2, std::length_error);
   }

   TEST(MatrixTest, MinusOperatorColumnSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}, {4}};
      EXPECT_THROW(m1-m2, std::length_error);
   }

   TEST(MatrixTest, MinusAssignmentOperator)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}};
      m1-=m2;
      EXPECT_EQ(m1.rowNumber(), 1);
      EXPECT_EQ(m1.columnNumber(), 2);
      EXPECT_EQ(m1.getData().at(0), std::vector<double>({0}));
      EXPECT_EQ(m1.getData().at(1), std::vector<double>({-2}));
   }

   TEST(MatrixTest, MinusAssignmentOperatorRowSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2,1}, {3,2}};
      EXPECT_THROW(m1-=m2, std::length_error);
   }

   TEST(MatrixTest, MinusAssignmentOperatorColumnSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}, {4}};
      EXPECT_THROW(m1-=m2, std::length_error);
   }


   TEST(MatrixTest, TimesOperator)
   {
      math::matrix<double> m1={{2,3}, {1,2}};
      math::matrix<double> m2={{2,4}};
      math::matrix<double> m3=m1*m2;
      EXPECT_EQ(m3.rowNumber(), 2);
      EXPECT_EQ(m3.columnNumber(), 1);
      EXPECT_EQ(m3.getData().at(0).at(0), 8);
      EXPECT_EQ(m3.getData().at(0).at(1), 14);
   }

   TEST(MatrixTest, TimesOperatorSize)
   {
      math::matrix<double> m1={{2,3}, {1,2}};
      math::matrix<double> m2={{2,4,3}};
      EXPECT_THROW(m1*m2, std::length_error);
   }

   TEST(MatrixTest, TimesAssignmentOperator)
   {
      math::matrix<double> m1={{2,3}, {1,2}};
      math::matrix<double> m2={{2,4}};
      m1*=m2;
      EXPECT_EQ(m1.rowNumber(), 2);
      EXPECT_EQ(m1.columnNumber(), 1);
      EXPECT_EQ(m1.getData().at(0).at(0), 8);
      EXPECT_EQ(m1.getData().at(0).at(1), 14);
   }

   TEST(MatrixTest, TimesAssignmentOperatorSize)
   {
      math::matrix<double> m1={{2,3}, {1,2}};
      math::matrix<double> m2={{2,4,3}};
      EXPECT_THROW(m1*=m2, std::length_error);
   }

   TEST(MatrixTest, DivideOperator)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {4}};
      math::matrix<double> m3=m1/m2;
      EXPECT_EQ(m3.rowNumber(), 1);
      EXPECT_EQ(m3.columnNumber(), 2);
      EXPECT_EQ(m3.getData().at(0), std::vector<double>({1}));
      EXPECT_EQ(m3.getData().at(1), std::vector<double>({0.25}));
   }

   TEST(MatrixTest, DivideOperatorRowSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2,1}, {3,2}};
      EXPECT_THROW(m1/m2, std::length_error);
   }

   TEST(MatrixTest, DivideOperatorColumnSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}, {4}};
      EXPECT_THROW(m1/m2, std::length_error);
   }

   TEST(MatrixTest, DivideOperatorZero)
   {
      math::matrix<double> m1={{1,2}, {2,3}};
      math::matrix<double> m2={{1,2}, {0,3}};
      EXPECT_THROW(m1/m2, std::domain_error);
   }

   TEST(MatrixTest, DivideAssignmentOperator)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {4}};
      m1/=m2;
      EXPECT_EQ(m1.rowNumber(), 1);
      EXPECT_EQ(m1.columnNumber(), 2);
      EXPECT_EQ(m1.getData().at(0), std::vector<double>({1}));
      EXPECT_EQ(m1.getData().at(1), std::vector<double>({0.25}));
   }

   TEST(MatrixTest, DivideAssignmentOperatorRowSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2,1}, {3,2}};
      EXPECT_THROW(m1/=m2, std::length_error);
   }

   TEST(MatrixTest, DivideAssignmentOperatorColumnSize)
   {
      math::matrix<double> m1={{2}, {1}};
      math::matrix<double> m2={{2}, {3}, {4}};
      EXPECT_THROW(m1/=m2, std::length_error);
   }

   TEST(MatrixTest, DivideAssignmentOperatorZero)
   {
      math::matrix<double> m1={{1,2}, {2,3}};
      math::matrix<double> m2={{1,2}, {0,3}};
      EXPECT_THROW(m1/=m2, std::domain_error);
   }

   TEST(MatrixTest, EqualsOperator)
   {
      math::matrix<double> m1={{2,3}, {1,2}};
      math::matrix<double> m2={{2,4,3},{1,2,3},{2,4,5}};
      math::matrix<double> m3={{2,4,3},{1,2,3},{2,4,5}};
      EXPECT_NE(m1, m2);
      EXPECT_EQ(m2, m3);
   }

   TEST(MatrixTest, WriteData)
   {
      std::vector<double> v={1,2,3};
      math::matrix<double> m={{0,6,7}, {1,4,5}};
      m.writeData(0, v);
      EXPECT_EQ(m.getData().at(0), std::vector<double>({1,2,3}));
      EXPECT_EQ(m.getData().at(1), std::vector<double>({1,4,5}));
   }

   TEST(MatrixTest, WriteDataIndex)
   {
      std::vector<double> v={1,2,3};
      math::matrix<double> m={{0,6,7}, {1,4,5}};
      EXPECT_THROW(m.writeData(2, v), std::range_error);
   }

   TEST(MatrixTest, GetSubMatrix)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4},{3,4,5}};
      math::matrix<double> m2=m1.getSubMatrix({1,2}, {0,1});
      EXPECT_EQ(m2, math::matrix<double>({{2,3},{3,4}}));
   }

   TEST(MatrixTest, GetSubMatrixRowSizeEqual)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      EXPECT_THROW(m.getSubMatrix({0,3}, {0,2}), std::range_error);
   }

   TEST(MatrixTest, GetSubMatrixRowSizeGreater)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      EXPECT_THROW(m.getSubMatrix({0,4}, {0,2}), std::range_error);
   }

   TEST(MatrixTest, GetSubMatrixColumnSizeEqual)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      EXPECT_THROW(m.getSubMatrix({0,2}, {0,3}), std::range_error);
   }

   TEST(MatrixTest, GetSubMatrixColumnSizeGreater)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      EXPECT_THROW(m.getSubMatrix({0,2}, {0,4}), std::range_error);
   }

   TEST(MatrixTest, Transpose)
   {
      math::matrix<double> m1={{1,2},{3,4},{5,6}};
      math::matrix<double> m2=m1.transpose();
      EXPECT_EQ(m2, math::matrix<double>({{1,3,5},{2,4,6}}));
   }

   TEST(MatrixTest, TransposeAssign)
   {
      math::matrix<double> m1={{1,2},{3,4},{5,6}};
      m1.transposeAssign();
      EXPECT_EQ(m1, math::matrix<double>({{1,3,5},{2,4,6}}));
   }

   TEST(MatrixTest, MultiplyElements)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4}};
      math::matrix<double> m2={{0,1,2},{1,2,3}};
      math::matrix<double> m3=m1.multiplyElements(m2);
      EXPECT_EQ(m3, math::matrix<double>({{0,2,6},{2,6,12}}));
   }

   TEST(MatrixTest, MultiplyElementsSize)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4}};
      math::matrix<double> m2={{0,1},{1,2}};
      EXPECT_THROW(m1.multiplyElements(m2), std::length_error);
   }

   TEST(MatrixTest, MultiplyElementsAssign)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4}};
      math::matrix<double> m2={{0,1,2},{1,2,3}};
      m1.multiplyElementsAssign(m2);
      EXPECT_EQ(m1, math::matrix<double>({{0,2,6},{2,6,12}}));
   }

   TEST(MatrixTest, MultiplyElementsAssignSize)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4}};
      math::matrix<double> m2={{0,1},{1,2}};
      EXPECT_THROW(m1.multiplyElementsAssign(m2), std::length_error);
   }

   TEST(MatrixTest, FriendTimesOperatorLeft)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4},{3,4,5}};
      math::complex c{2,1};
      math::matrix<double> m2=m1*c;
      EXPECT_EQ(m2.rowNumber(), 3);
      EXPECT_EQ(m2.columnNumber(), 3);
      EXPECT_EQ(m2.getData().at(0), std::vector<double>({2,4,6}));
      EXPECT_EQ(m2.getData().at(1), std::vector<double>({4,6,8}));
      EXPECT_EQ(m2.getData().at(2), std::vector<double>({6,8,10}));
   }

   TEST(MatrixTest, FriendTimesOperatorRight)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4},{3,4,5}};
      math::complex c{2,1};
      math::matrix<double> m2=c*m1;
      EXPECT_EQ(m2.rowNumber(), 3);
      EXPECT_EQ(m2.columnNumber(), 3);
      EXPECT_EQ(m2.getData().at(0), std::vector<double>({2,4,6}));
      EXPECT_EQ(m2.getData().at(1), std::vector<double>({4,6,8}));
      EXPECT_EQ(m2.getData().at(2), std::vector<double>({6,8,10}));
   }

   TEST(MatrixTest, FriendTimesAssignmentOperator)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      math::complex c{2,1};
      m*=c;
      EXPECT_EQ(m.rowNumber(), 3);
      EXPECT_EQ(m.columnNumber(), 3);
      EXPECT_EQ(m.getData().at(0), std::vector<double>({2,4,6}));
      EXPECT_EQ(m.getData().at(1), std::vector<double>({4,6,8}));
      EXPECT_EQ(m.getData().at(2), std::vector<double>({6,8,10}));
   }

   TEST(MatrixTest, FriendDivideOperator)
   {
      math::matrix<double> m1={{1,2,3},{2,3,4},{3,4,5}};
      double d=2;
      math::matrix<double> m2=m1/d;
      EXPECT_EQ(m2.rowNumber(), 3);
      EXPECT_EQ(m2.columnNumber(), 3);
      EXPECT_EQ(m2.getData().at(0), std::vector<double>({0.5,1,1.5}));
      EXPECT_EQ(m2.getData().at(1), std::vector<double>({1,1.5,2}));
      EXPECT_EQ(m2.getData().at(2), std::vector<double>({1.5,2,2.5}));
   }

   TEST(MatrixTest, FriendDivideOperatorZero)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      double d=0;
      EXPECT_THROW(m/d, std::domain_error);
   }

   TEST(MatrixTest, FriendDivideAssignmentOperator)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      double d=2;
      m/=d;
      EXPECT_EQ(m.rowNumber(), 3);
      EXPECT_EQ(m.columnNumber(), 3);
      EXPECT_EQ(m.getData().at(0), std::vector<double>({0.5,1,1.5}));
      EXPECT_EQ(m.getData().at(1), std::vector<double>({1,1.5,2}));
      EXPECT_EQ(m.getData().at(2), std::vector<double>({1.5,2,2.5}));
   }

   TEST(MatrixTest, FriendDivideAssignmentOperatorZero)
   {
      math::matrix<double> m={{1,2,3},{2,3,4},{3,4,5}};
      double d=0;
      EXPECT_THROW(m/=d, std::domain_error);
   }
}
