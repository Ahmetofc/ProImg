namespace image
{
   inline double clamp(double value)
   {
      if(value<0) return 0;
      if(value>1) return 1;
      return value;
   }

   inline double gammaCorrection(u8 channel)
   {
      double normalized = static_cast<double>(channel)/255;
      return std::pow(normalized, 2.2);
   }

   inline u8 inverseGammaCorrection(double channel)
   {
      channel=std::pow(clamp(channel), 1/2.2);
      return channel*255;
   }

   inline std::string ppmGetline(std::ifstream& image)
   {
      std::string buffer{};
      u8 ch;
      while((ch=image.get())!=' ' && ch!='\n')
         if(ch=='#')
            image.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         else
            buffer+=ch;
      return buffer;
   }

   inline void ppmHeader(std::ofstream& image, const ppmArgs args)
   {
      if(args.maxVal!=255)
         throw std::range_error("DSP only supports 8-bit precision per channel.\n");
      image << args.type << "\n" << args.width << " "  << args.height << "\n" << args.maxVal << "\n";
   }

   inline ppmArgs ppmHeaderInfo(std::ifstream& image)
   {
      ppmArgs args;
      std::string temp;
      u8 count{};

      while(count!=4)
      {
         if(!count)
         {
            std::getline(image, temp);
            if(temp[0]!='#')
            {
               args.type=temp;
               count++;
            }
         }
         if(count==1)
         {
            std::getline(image, temp, ' ');
            if(temp[0]!='#')
            {
               args.width=atoi(temp.c_str());
               count++;
            }
            else
               std::getline(image, temp);
         }
         if(count==2)
         {
            std::getline(image, temp);
            if(temp[0]!='#')
            {
               args.height=atoi(temp.c_str());
               count++;
            }
         }
         if(count==3)
         {
            std::getline(image, temp);
            if(temp[0]!='#')
            {
               args.maxVal=atoi(temp.c_str());
               count++;
            }
         }
      }

      if(args.maxVal!=255)
         throw std::range_error("DSP only supports 8-bit precision per channel.\n");

      return args;
   }

   inline void ppmData(std::ofstream& image, const math::matrix<double>& pixels, const ppmArgs args)
   {
      if(pixels.rowNumber()!=1 && pixels.rowNumber()!=3)
         throw std::length_error("Invalid pixel entries.\n");
      auto data = pixels.getData();

      if(args.maxVal!=255)
         throw std::range_error("DSP only supports 8-bit precision per channel.\n");

      for(const auto& column : data)
         for(const auto& element : column)
            image << inverseGammaCorrection(element);
   }

   inline math::matrix<double> ppmExtractData(std::ifstream& image, const ppmArgs args)
   {
      math::matrix<double> res(3, args.width*args.height);
      u8 r,g,b;
      for(u32 index{}; index<args.width*args.height; index++)
      {
         r=image.get();
         g=image.get();
         b=image.get();
         res.writeData(index, {gammaCorrection(r),gammaCorrection(g),gammaCorrection(b)});
      }
      return res;
   }
}
