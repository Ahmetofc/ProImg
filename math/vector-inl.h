namespace math
{
   template <typename T>
   constexpr vector<T>::vector(u32 _dim) noexcept : dim(_dim), data(std::vector<T>(_dim, 0))
   {
   }

   template <typename T>
   constexpr vector<T>::vector(const std::initializer_list<T>& _data) noexcept : dim(_data.size()), data(std::vector<T>{_data})
   {
   }

   template <typename T>
   constexpr vector<T>::vector(const std::vector<T>& _data) noexcept : dim(_data.size()), data(std::vector<T>{_data})
   {
   }

   template <typename T>
   template <typename R>
   constexpr vector<T>::vector(const std::vector<R>& rhs)
   {
      dim=rhs.size();
      data=std::vector<T>(dim);
      for(u32 i{}; i<dim; i++)
         data[i]=rhs.at(i);
   }

   template <typename T>
   constexpr vector<T>::vector(const vector<T>& rhs) noexcept : dim(rhs.dim), data(rhs.data)
   {
   }

   template <typename T>
   vector<T>& vector<T>::operator=(const vector<T>& rhs) noexcept
   {
      dim=rhs.dim;
      data=rhs.data;
      return *this;
   }

   template <typename T>
   vector<T> vector<T>::operator+(const vector<T>& rhs) const
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      vector res=*this;
      for(u32 i{}; i<dim; i++)
         res.data[i]+=rhs.data.at(i);
      return res;
   }

   template <typename T>
   vector<T>& vector<T>::operator+=(const vector<T>& rhs) 
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      for(u32 i{}; i<dim; i++)
         data[i]+=rhs.data.at(i);
      return *this;
   }

   template <typename T>
   vector<T> vector<T>::operator-(const vector<T>& rhs) const
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      vector res=*this;
      for(u32 i{}; i<dim; i++)
         res.data[i]-=rhs.data.at(i);
      return res;
   }

   template <typename T>
   vector<T>& vector<T>::operator-=(const vector<T>& rhs) 
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      for(u32 i{}; i<dim; i++)
         data[i]-=rhs.data.at(i);
      return *this;
   }

   template <typename T>
   complex vector<T>::operator*(const vector<T>& rhs) const
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      complex res{};
      for(u32 i{}; i<dim; i++)
         res+=data.at(i)*rhs.data.at(i);
      return res;
   }

   template <typename T>
   vector<T> vector<T>::operator/(const vector<T>& rhs) const
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      vector res=*this;
      for(u32 i{}; i<dim; i++)
      {
         if(rhs.data.at(i)==0)
            throw std::domain_error("Division by zero.\n");
         res.data[i]/=rhs.data.at(i);
      }
      return res;
   }

   template <typename T>
   vector<T>& vector<T>::operator/=(const vector<T>& rhs) 
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      for(u32 i{}; i<dim; i++)
      {
         if(rhs.data.at(i)==0)
            throw std::domain_error("Division by zero.\n");
         data[i]/=rhs.data.at(i);
      }
      return *this;
   }

   template <typename T>
   bool vector<T>::operator==(const vector<T>& rhs) const noexcept
   {
      return dim==rhs.dim && data==rhs.data;
   }

   template <typename T>
   vector<T>::operator std::vector<T>() const noexcept
   {
      return data;
   }

   template <typename T>
   constexpr u32 vector<T>::dimension() const noexcept
   {
      return dim;
   }

   template <typename T>
   constexpr std::vector<T> vector<T>::getData() const noexcept
   {
      return data;
   }

   template <typename T>
   constexpr void vector<T>::writeData(u32 index, T element)
   {
      if(index>=dim)
         throw std::range_error("Index out of range.\n");
      data[index]=element;
   }

   template <typename T>
   constexpr double vector<T>::magnitude() const noexcept
   {
      double sum{};
      if constexpr(std::is_same_v<T, math::complex>)
         for(const auto& element : data)
            sum+=element.normSquared();
      else
         for(const auto& element : data)
            sum+=element*element;
      return std::sqrt(sum);
   }

   template <typename T>
   constexpr double vector<T>::magnitudeSquared() const noexcept
   {
      double sum{};
      if constexpr(std::is_same_v<T, math::complex>)
         for(const auto& element : data)
            sum+=element.normSquared();
      else
         for(const auto& element : data)
            sum+=element*element;
      return sum;
   }

   template <typename T>
   vector<T> vector<T>::multiplyElements(const vector<T>& rhs) const
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      vector res=*this;
      for(u32 i{}; i<dim; i++)
         res.data[i]*=rhs.data.at(i);
      return res;
   }

   template <typename T>
   vector<T>& vector<T>::multiplyElementsAssign(const vector<T>& rhs) 
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      for(u32 i{}; i<dim; i++)
         data[i]*=rhs.data.at(i);
      return *this;
   }

   template <typename T>
   vector<T> vector<T>::crossProduct(const vector<T>& rhs) const
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      if(dim!=3)
         throw std::domain_error("Cross product not defined for vector dimensions.\n");
      T v1=data.at(1)*rhs.data.at(2)-data.at(2)*rhs.data.at(1);
      T v2=data.at(2)*rhs.data.at(0)-data.at(0)*rhs.data.at(2);
      T v3=data.at(0)*rhs.data.at(1)-data.at(1)*rhs.data.at(0);
      vector res({v1, v2, v3});
      return res;
   }

   template <typename T>
   vector<T>& vector<T>::crossProductAssign(const vector<T>& rhs) 
   {
      if(dim!=rhs.dim)
         throw std::length_error("Vectors are incompatible.\n");
      if(dim!=3)
         throw std::domain_error("Cross product not defined for vector dimensions.\n");
      T v1=data.at(1)*rhs.data.at(2)-data.at(2)*rhs.data.at(1);
      T v2=data.at(2)*rhs.data.at(0)-data.at(0)*rhs.data.at(2);
      T v3=data.at(0)*rhs.data.at(1)-data.at(1)*rhs.data.at(0);
      data={v1, v2, v3};
      return *this;
   }
}