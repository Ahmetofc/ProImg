namespace math
{
   inline matrix<double> convolve1D(const matrix<double>& signal, const matrix<double>& mask) noexcept
   {
      u32 dist=mask.columnNumber()/2;
      auto input_signal=signal.getData();
      auto mask_data=mask.getData();
      auto output_signal=input_signal;
      for(u32 i{}; i<signal.columnNumber(); i++)
         for(u32 j{}; j<signal.rowNumber(); j++)
         {
            double val{};
            for(u32 k{}; k<mask.columnNumber(); k++)
            {
               try
               {
                  val+=input_signal.at(i).at(j+k-dist)*mask_data.at(mask.columnNumber()-k-1).at(0);
               }
               catch(...)
               {
               }
            }
            output_signal.at(i).at(j)=val;
         }
      return output_signal;
   }

   inline matrix<double> convolve2D(const matrix<double>& signal, const matrix<double>& mask1, const matrix<double>& mask2) noexcept
   {
      auto convolve1=convolve1D(signal, mask1);
      auto convolve2=convolve1D(convolve1.transpose(), mask2);
      return convolve2.transpose();
   }

   inline matrix<double> convolve2D(const matrix<double>& signal, const matrix<double>& mask) noexcept
   {
      auto input_signal=signal.getData();
      auto mask_data=mask.getData();
      auto output_signal=input_signal;
      u32 dist1=mask.rowNumber()/2;
      u32 dist2=mask.columnNumber()/2;
      for(u32 i{}; i<signal.columnNumber(); i++)
         for(u32 j{}; j<signal.rowNumber(); j++)
         {
            double val{};
            for(u32 k{}; k<mask.columnNumber(); k++)
               for(u32 l{}; l<mask.rowNumber(); l++)
               {
                  try
                  {
                     val+=input_signal.at(i+l-dist1).at(j+k-dist2)*mask_data.at(mask.columnNumber()-k-1).at(mask.rowNumber()-l-1);
                  }
                  catch(...)
                  {
                  }
               }
            output_signal.at(i).at(j)=val;
         }
      return output_signal;
   }

   template <typename T>
   inline matrix<T> get2DMask(const vector<T>& mask1, const vector<T>& mask2)
   {
      matrix mask2d(mask1.dimension(), mask2.dimension());
      auto mask1_data=mask1.getData();
      auto mask2_data=mask2.getData();
      for(u32 i{}; i<mask2.dimension(); i++)
         mask2d.writeData(i, mask2_data.at(i)*mask1_data);
   }
}
