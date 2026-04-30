namespace math
{
   template <typename T>
   constexpr matrix<T>::matrix(u32 _row, u32 _col) noexcept : row(_row), col(_col), data(std::vector<std::vector<T>>{_col})
   {
      for(auto& column : data)
         column=std::vector<T>(row, 0);
   }

   template <typename T>
   constexpr matrix<T>::matrix(const std::initializer_list<std::initializer_list<T>>& _data) : row(std::data(_data)[0].size()), col(_data.size()), data(std::vector<std::vector<T>>{_data.size()})
   {
      size_t _size=std::data(_data)[0].size();
      for(u32 i{}; i<col; i++)
      {
         if(std::data(_data)[i].size()!=_size)
            throw std::length_error("Invalid matrix entries.\n");
         data[i]=std::data(_data)[i];
      }
   }

   template <typename T>
   constexpr matrix<T>::matrix(const std::vector<std::vector<T>>& _data) : row(_data.at(0).size()), col(_data.size()), data(std::vector<std::vector<T>>{_data.size()})
   {
      size_t _size=_data.at(0).size();
      for(u32 i{}; i<col; i++)
      {
         if(_data.at(i).size()!=_size)
            throw std::length_error("Invalid matrix entries.\n");
         data[i]=_data.at(i);
      }
   }

   template <typename T>
   constexpr matrix<T>::matrix(const matrix<T>& rhs) noexcept : row(rhs.row), col(rhs.col), data(rhs.data)
   {
   }

   template <typename T>
   matrix<T>& matrix<T>::operator=(const matrix<T>& rhs) noexcept
   {
      row=rhs.row;
      col=rhs.col;
      data=rhs.data;
      return *this;
   }

   template <typename T>
   matrix<T> matrix<T>::operator+(const matrix<T>& rhs) const
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      matrix res=*this;
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
            res.data[i][j]+=rhs.data.at(i).at(j);
      return res;
   }

   template <typename T>
   matrix<T>& matrix<T>::operator+=(const matrix<T>& rhs) 
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
            data[i][j]+=rhs.data.at(i).at(j);
      return *this;
   }

   template <typename T>
   matrix<T> matrix<T>::operator-(const matrix<T>& rhs) const
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      matrix res=*this;
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
            res.data[i][j]-=rhs.data.at(i).at(j);
      return res;
   }

   template <typename T>
   matrix<T>& matrix<T>::operator-=(const matrix<T>& rhs) 
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
            data[i][j]-=rhs.data.at(i).at(j);
      return *this;
   }

   template <typename T>
   matrix<T> matrix<T>::operator*(const matrix<T>& rhs) const
   {
      if(col!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      matrix res{row, rhs.col};
      for(u32 i{}; i<rhs.col; i++)
         for(u32 k{}; k<col; k++)
            for(u32 j{}; j<row; j++)
               res.data[i][j]+=data[k][j]*rhs.data[i][k];
      return res;
   }

   template <typename T>
   matrix<T>& matrix<T>::operator*=(const matrix<T>& rhs) 
   {
      if(col!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      matrix res{row, rhs.col};
      for(u32 i{}; i<rhs.col; i++)
         for(u32 j{}; j<row; j++)
            for(u32 k{}; k<col; k++)
               res.data[i][j]+=data[k][j]*rhs.data[i][k];
      col=rhs.col;
      data=res.data;
      return *this;
   }

   template <typename T>
   matrix<T> matrix<T>::operator/(const matrix<T>& rhs) const
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      matrix res=*this;
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
         {
            if(rhs.data.at(i).at(j)==0)
               throw std::domain_error("Division by zero.\n");
            res.data[i][j]/=rhs.data.at(i).at(j);
         }
      return res;
   }

   template <typename T>
   matrix<T>& matrix<T>::operator/=(const matrix<T>& rhs) 
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
         {
            if(rhs.data.at(i).at(j)==0)
               throw std::domain_error("Division by zero.\n");
            data[i][j]/=rhs.data.at(i).at(j);
         }
      return *this;
   }

   template <typename T>
   bool matrix<T>::operator==(const matrix<T>& rhs) const noexcept
   {
      return row==rhs.row && col==rhs.col && data==rhs.data;
   }

   template <typename T>
   constexpr u32 matrix<T>::rowNumber() const noexcept
   {
      return row;
   }

   template <typename T>
   constexpr u32 matrix<T>::columnNumber() const noexcept
   {
      return col;
   }

   template <typename T>
   constexpr std::vector<std::vector<T>> matrix<T>::getData() const noexcept
   {
      return data;
   }

   template <typename T>
   constexpr void matrix<T>::writeData(u32 index, const std::vector<T>& column)
   {
      if(index>=col)
         throw std::range_error("Index out of range.\n");
      data[index]=column;
   }

   template <typename T>
   matrix<T> matrix<T>::getSubMatrix(intRange rows, intRange cols) const
   {
      if(row<=rows.end || col<=cols.end)
         throw std::range_error("Invalid range(s).\n");
      matrix res{rows.span(), cols.span()};
      for(u32 i{cols.start}; i<cols.end+1; i++)
         for(u32 j{rows.start}; j<rows.end+1; j++)
            res.data[i-cols.start][j-rows.start]=data.at(i).at(j);
      return res;
   }

   template <typename T>
   matrix<T> matrix<T>::transpose() const noexcept
   {
      matrix transp(col, row);
      for(u32 i{}; i<row; i++)
         for(u32 j{}; j<col; j++)
            transp.data[i][j]=data.at(j).at(i);
      return transp;
   }

   template <typename T>
   matrix<T>& matrix<T>::transposeAssign() noexcept
   {
      matrix transp(col, row);
      for(u32 i{}; i<row; i++)
         for(u32 j{}; j<col; j++)
            transp.data[i][j]=data.at(j).at(i);
      row=transp.row;
      col=transp.col;
      data=transp.data;
      return *this;
   }

   template <typename T>
   matrix<T> matrix<T>::multiplyElements(const matrix<T>& rhs) const
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      matrix res=*this;
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
            res.data[i][j]*=rhs.data.at(i).at(j);
      return res;
   }

   template <typename T>
   matrix<T>& matrix<T>::multiplyElementsAssign(const matrix<T>& rhs)
   {
      if(col!=rhs.col || row!=rhs.row)
         throw std::length_error("Matrices are incompatible.\n");
      for(u32 i{}; i<col; i++)
         for(u32 j{}; j<row; j++)
            data[i][j]*=rhs.data.at(i).at(j);
      return *this;
   }
}
