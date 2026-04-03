namespace image
{
   inline math::matrix<double> extractR(const math::matrix<double>& data) noexcept
   {
      u32 max_iter=data.columnNumber();
      math::matrix<double> channel(1, max_iter);
      auto data_=data.getData();
      for(u32 i{}; i<data.columnNumber(); i++)
      {
         channel.writeData(i, {data_.at(i).at(0)});
      }
      return channel;
   }

   inline math::matrix<double> extractG(const math::matrix<double>& data) noexcept
   {
      u32 max_iter=data.columnNumber();
      math::matrix<double> channel(1, max_iter);
      auto data_=data.getData();
      for(u32 i{}; i<data.columnNumber(); i++)
      {
         channel.writeData(i, {data_.at(i).at(1)});
      }
      return channel;
   }

   inline math::matrix<double> extractB(const math::matrix<double>& data) noexcept
   {
      u32 max_iter=data.columnNumber();
      math::matrix<double> channel(1, max_iter);
      auto data_=data.getData();
      for(u32 i{}; i<data.columnNumber(); i++)
      {
         channel.writeData(i, {data_.at(i).at(2)});
      }
      return channel;
   }

   inline math::matrix<double> extractY(const math::matrix<double>& data) noexcept
   {
      u32 max_iter=data.columnNumber();
      math::matrix<double> channel(1, max_iter);
      auto data_=data.getData();
      for(u32 i{}; i<max_iter; i++)
         channel.writeData(i, {math::vector<double>({0.299, 0.587, 0.114})*data_.at(i)});
      return channel;
   }

   inline math::matrix<double> extractCb(const math::matrix<double>& data) noexcept
   {
      u32 max_iter=data.columnNumber();
      math::matrix<double> channel(1, max_iter);
      auto data_=data.getData();
      for(u32 i{}; i<data.columnNumber(); i++)
         channel.writeData(i, {0.5+math::vector<double>({-0.169, -0.330, 0.499})*data_.at(i)});
      return channel;
   }

   inline math::matrix<double> extractCr(const math::matrix<double>& data) noexcept
   {
      u32 max_iter=data.columnNumber();
      math::matrix<double> channel(1, max_iter);
      auto data_=data.getData();
      for(u32 i{}; i<data.columnNumber(); i++)
         channel.writeData(i, {0.5+math::vector<double>({0.500, -0.419, 0.081})*data_.at(i)});
      return channel;
   }

   inline math::matrix<double> RGBToYCbCr(const math::matrix<double>& rgb)
   {
      if(rgb.rowNumber()!=3)
         throw std::domain_error("Invalid RGB data.\n");
      math::matrix<double> ycbcr(3, rgb.columnNumber());
      const auto rgb_data=rgb.getData();
      for(u32 i{}; i<rgb.columnNumber(); i++)
         ycbcr.writeData(i, {0.299*rgb_data.at(i).at(0)+0.587*rgb_data.at(i).at(1)+0.114*rgb_data.at(i).at(2), 
                             0.5-0.169*rgb_data.at(i).at(0)-0.330*rgb_data.at(i).at(1)+0.499*rgb_data.at(i).at(2),
                             0.5+0.500*rgb_data.at(i).at(0)-0.419*rgb_data.at(i).at(1)-0.081*rgb_data.at(i).at(2)});
      return ycbcr;
   }

   inline math::matrix<double> RGBToYCbCr(const math::matrix<double>& r, const math::matrix<double>& g, const math::matrix<double>& b)
   {
      if(r.rowNumber()!=1)
         throw std::domain_error("Invalid luma channel.\n");
      if(g.rowNumber()!=1)
         throw std::domain_error("Invalid chroma blue channel.\n");
      if(b.rowNumber()!=1)
         throw std::domain_error("Invalid chroma red channel.\n");
      if(r.columnNumber()!=g.columnNumber() || r.columnNumber()!=b.columnNumber() || g.columnNumber()!=b.columnNumber())
         throw std::length_error("Incompatible channels.\n");
      math::matrix<double> ycbcr(3, r.columnNumber());
      const auto r_data=r.getData();
      const auto g_data=g.getData();
      const auto b_data=b.getData();
      for(u32 i{}; i<r.columnNumber(); i++)
         ycbcr.writeData(i, {0.299*r_data.at(i).at(0)+0.587*g_data.at(i).at(0)+0.114*b_data.at(i).at(0), 
                             0.5-0.169*r_data.at(i).at(0)-0.330*g_data.at(i).at(0)+0.499*b_data.at(i).at(0),
                             0.5+0.500*r_data.at(i).at(0)-0.419*g_data.at(i).at(0)-0.081*b_data.at(i).at(0)});
      return ycbcr;
   }

   inline math::matrix<double> YCbCrToRGB(const math::matrix<double>& ycbcr)
   {
      if(ycbcr.rowNumber()!=3)
         throw std::domain_error("Invalid YCbCr data.\n");
      math::matrix<double> rgb(3, ycbcr.columnNumber());
      const auto ycbcr_data=ycbcr.getData();
      for(u32 i{}; i<ycbcr.columnNumber(); i++)
      {
         rgb.writeData(i, {ycbcr_data.at(i).at(0)+1.403*(ycbcr_data.at(i).at(2)-0.5), 
                           ycbcr_data.at(i).at(0)-0.714*(ycbcr_data.at(i).at(2)-0.5)-0.344*(ycbcr_data.at(i).at(1)-0.5), 
                           ycbcr_data.at(i).at(0)+1.772*(ycbcr_data.at(i).at(1)-0.5)});
      }
      return rgb;
   }

   inline math::matrix<double> YCbCrToRGB(const math::matrix<double>& y, const math::matrix<double>& cb, const math::matrix<double>& cr)
   {
      if(y.rowNumber()!=1)
         throw std::domain_error("Invalid luma channel.\n");
      if(cb.rowNumber()!=1)
         throw std::domain_error("Invalid chroma blue channel.\n");
      if(cr.rowNumber()!=1)
         throw std::domain_error("Invalid chroma red channel.\n");
      if(y.columnNumber()!=cb.columnNumber() || y.columnNumber()!=cr.columnNumber() || cb.columnNumber()!=cr.columnNumber())
         throw std::length_error("Incompatible channels.\n");
      math::matrix<double> rgb(3, y.columnNumber());
      const auto y_data=y.getData();
      const auto cb_data=cb.getData();
      const auto cr_data=cr.getData();
      for(u32 i{}; i<y.columnNumber(); i++)
         rgb.writeData(i, {y_data.at(i).at(0)+1.403*(cr_data.at(i).at(0)-0.5), 
                           y_data.at(i).at(0)-0.714*(cr_data.at(i).at(0)-0.5)-0.344*(cb_data.at(i).at(0)-0.5), 
                           y_data.at(i).at(0)+1.772*(cb_data.at(i).at(0)-0.5)});
      return rgb;
   }

   inline void visualize(const math::matrix<double>& data, std::string mode, u16 width, u16 height)
   {
      if(data.rowNumber()*data.columnNumber()!=width*height)
         throw std::length_error("Invalid width/height.\n");
      std::ofstream output(std::string("test-")+mode+".ppm");
      auto data_ = data.getData();
      if(mode=="r")
      {
         math::matrix<double> pixels(3, width*height);
         ppmHeader(output, {.width=width, .height=height});
         for(u32 i{}; i<width*height; i++)
            pixels.writeData(i, {data_.at(i).at(0), 0, 0});
         ppmData(output, pixels);
      }
      else if(mode=="g")
      {
         math::matrix<double> pixels(3, width*height);
         ppmHeader(output, {.width=width, .height=height});
         for(u32 i{}; i<width*height; i++)
            pixels.writeData(i, {0, data_.at(i).at(0), 0});
         ppmData(output, pixels);
      }
      else if(mode=="b")
      {
         math::matrix<double> pixels(3, width*height);
         ppmHeader(output, {.width=width, .height=height});
         for(u32 i{}; i<width*height; i++)
            pixels.writeData(i, {0, 0, data_.at(i).at(0)});
         ppmData(output, pixels);
      }
      else if(mode=="y")
      {
         ppmHeader(output, {.type="P5", .width=width, .height=height});
         ppmData(output, data);
      }
      else if(mode=="cb")
      {
         math::matrix<double> pixels(3, width*height);
         ppmHeader(output, {.width=width, .height=height});
         for(u32 i{}; i<width*height; i++)
         {
            if(data_.at(i).at(0)>0.5)
               pixels.writeData(i, {0, 0, data_.at(i).at(0)});
            else
               pixels.writeData(i, {1-data_.at(i).at(0), 1-data_.at(i).at(0), 0});
         }
         ppmData(output, pixels);
      }
      else if(mode=="cr")
      {
         math::matrix<double> pixels(3, width*height);
         ppmHeader(output, {.width=width, .height=height});
         for(u32 i{}; i<width*height; i++)
         {
            if(data_.at(i).at(0)>0.5)
               pixels.writeData(i, {data_.at(i).at(0), 0, 0});
            else
               pixels.writeData(i, {0, 1-data_.at(i).at(0), 1-data_.at(i).at(0)});
         }
         ppmData(output, pixels);
      }
      else
         throw std::domain_error("Invalid mode, valid modes are: \"r\", \"g\", \"b\", \"y\", \"cb\", \"cr\".\n");
   }

   inline math::matrix<double> exposure(const math::matrix<double>& data, double exp) noexcept
   {
      auto exposed_data=data.getData();
      for(auto& column : exposed_data)
         for(auto& element : column)
            element*=std::pow(2, exp);
      return exposed_data;
   }

   inline math::matrix<double> exposure(const math::matrix<double>& data, double exp1, double exp2, double exp3) 
   {
      if(data.rowNumber()!=3)
         throw std::range_error("Non-matching channels and exponents.\n");
      auto exposed_data=data.getData();
      for(auto& column : exposed_data)
      {
         column[0]*=std::pow(2, exp1);
         column[1]*=std::pow(2, exp2);
         column[2]*=std::pow(2, exp3);
      }
      return exposed_data;
   }

   inline math::matrix<double> saturation(const math::matrix<double>& data, double scale) 
   {
      if(scale<0)
         throw std::domain_error("Invalid saturation factor.\n");
      auto saturated_data=data.getData();
      if(data.rowNumber()==1)
         for(auto& column : saturated_data)
               column[0]=0.5+(column.at(0)-0.5)*scale;
      else if(data.rowNumber()==3)
         for(auto& column : saturated_data)
         {
            column[1]=0.5+(column.at(1)-0.5)*scale;
            column[2]=0.5+(column.at(2)-0.5)*scale;
         }
      else
         throw std::domain_error("Invalid channel(s).\n");
      return saturated_data;
   }

   inline math::matrix<double> saturation(const math::matrix<double>& data, double scaleBlue, double scaleRed) 
   {
      if(data.rowNumber()!=3)
         throw std::range_error("Non-matching channels and factors.\n");
      if(scaleRed<0 || scaleBlue<0)
         throw std::domain_error("Invalid saturation factor.\n");
      auto saturated_data=data.getData();
      for(auto& column : saturated_data)
      {
         column[1]=0.5+(column.at(1)-0.5)*scaleBlue;
         column[2]=0.5+(column.at(2)-0.5)*scaleRed;
      }
      return saturated_data;
   }

   inline math::matrix<double> contrast(const math::matrix<double>& data, double contrast)
   {
      if(data.rowNumber()!=1 && data.rowNumber()!=3)
         throw std::range_error("Invalid channels.\n");
      if(contrast<0)
         throw std::domain_error("Invalid contrast.\n");
      auto contrast_data=data.getData();
      for(auto& column : contrast_data)
         column[0]=0.5+(column.at(0)-0.5)*contrast;
      return contrast_data;
   }

   inline math::matrix<double> gaussianBlur(const math::matrix<double>& data, double sigma, u32 width, u32 height)
   {
      auto ch1 = extractR(data); auto ch1_data = ch1.getData();
      auto ch2 = extractG(data); auto ch2_data = ch2.getData();
      auto ch3 = extractB(data); auto ch3_data = ch3.getData();
      math::matrix<double> m1(height, width);
      math::matrix<double> m2(height, width);
      math::matrix<double> m3(height, width);
      math::vector<double> v1(height);
      math::vector<double> v2(height);
      math::vector<double> v3(height);
      for(u32 i{}; i<width; i++)
      {
         for(u32 j{}; j<height; j++)
         {
            v1.writeData(j, ch1_data.at(i+j*width).at(0));
            v2.writeData(j, ch2_data.at(i+j*width).at(0));
            v3.writeData(j, ch3_data.at(i+j*width).at(0));
         }
         m1.writeData(i, v1);
         m2.writeData(i, v2);
         m3.writeData(i, v3);
      }
      u32 kernel_size = 2*M_PI*sigma;
      auto gaussianFilter = [sigma](double x, double y)->double{ return std::pow(M_E, -(x*x+y*y)/(2*sigma*sigma)); };
      double norm{};
      for(u32 i{}; i<kernel_size; i++)
         for(u32 j{}; j<kernel_size; j++)
            norm+=gaussianFilter(i-kernel_size/2, j-kernel_size/2);
      math::matrix<double> kernel(kernel_size, kernel_size);
      math::vector<double> v(kernel_size);
      for(u32 i{}; i<kernel_size; i++)
      {
         for(u32 j{}; j<kernel_size; j++)
            v.writeData(j, gaussianFilter(i-kernel_size/2, j-kernel_size/2)/norm);
         kernel.writeData(i, v);
      }
      auto ch1_blur = math::convolve2D(m1, kernel).getData();
      auto ch2_blur = math::convolve2D(m2, kernel).getData();
      auto ch3_blur = math::convolve2D(m3, kernel).getData();
      math::matrix<double> blur_data(3, width*height);
      for(u32 i{}; i<width; i++)
         for(u32 j{}; j<height; j++)
            blur_data.writeData(i+j*width, {ch1_blur.at(i).at(j), ch2_blur.at(i).at(j), ch3_blur.at(i).at(j)});
      return blur_data;
   }
}
