#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>

#include "tools/cpp/runfiles/runfiles.h"
#include "SDL2/SDL.h"

#include "app/log-inl.h"
#include "image/effects.h"
#include "image/ppm.h"
#include "math/convolution.h"
#include "math/matrix.h"
#include "math/random.h"
#include "math/vector.h"

inline constexpr u16 STACK_SIZE = 10;

using bazel::tools::cpp::runfiles::Runfiles;
using Callback = std::function<void(const SDL_Event&)>;

int main(int argc, char* argv[])
{
   if(argc!=2)
   {
      std::cerr << "Invalid use.\nUsage: bazel run //app:DSP -- <filename>\nNote: <filename> should exist in the data/ directory." << std::endl;
      return -1;
   }
   std::string runfiles_error;
   std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0], &runfiles_error));
   if(!runfiles)
   {
      std::cerr << runfiles_error << std::endl;
      return -1;
   }
   std::string image_path = runfiles->Rlocation("dsp/data/")+argv[1];
   std::ifstream ppm(image_path, std::ios::binary);
   if(!ppm.is_open())
   {
      std::cerr << "Unable to open the image file." << std::endl;
      return -1;
   }

   std::string amount_buffer{};
   std::string name_buffer{};
   std::string name_buffer_prev = argv[1];
   std::string project_root = std::getenv("BUILD_WORKSPACE_DIRECTORY");
   bool is_set_buffer = false;
   bool is_revert = false;
   bool updated_amount_buffer = false;
   bool updated_name_buffer = false;
   bool is_set_name = false;
   std::vector<actionLog> undo_redo_stack;
   std::vector<actionLog> action_stack;
   undo_redo_stack.reserve(STACK_SIZE);
   action_stack.reserve(STACK_SIZE*4);
   u16 log_index{};

   image::ppmArgs args=image::ppmHeaderInfo(ppm);
   math::matrix<double> rgbData = image::ppmExtractData(ppm, args);
   math::matrix<double> rgbDataLog = rgbData;
   math::matrix<double> rgbDataLogCopy;
   math::matrix<double> ycbcrData;
   std::vector<u8> pixelData;
   pixelData = image::sdlPreprocess(rgbData);

   if(!std::filesystem::is_directory(project_root+"/out"))
      std::filesystem::create_directory(project_root+"/out");
   if(!std::filesystem::is_directory(project_root+"/logs"))
      std::filesystem::create_directory(project_root+"/logs");

   if(SDL_Init(SDL_INIT_VIDEO))
   {
      std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
      return -1;
   }
   SDL_Window* window = SDL_CreateWindow((std::string("ProImg - ")+argv[1]).c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, args.width, args.height, SDL_WINDOW_SHOWN);
   if(!window)
   {
      std::cerr << "Error in window initialization: " << SDL_GetError() << std::endl;
      return -1;
   }
   SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
   if(!renderer)
   {
      std::cerr << "Error in renderer initialization: " << SDL_GetError() << std::endl;
      SDL_DestroyWindow(window);
      return -1;
   }
   SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, args.width, args.height);
   if(!texture)
   {
      std::cerr << "Error in texture initialization: " << SDL_GetError() << std::endl;
      SDL_DestroyWindow(window);
      SDL_DestroyRenderer(renderer);
      return -1;
   }
   bool quit = false;
   SDL_Event e;

   Callback contrast = [&](const SDL_Event& e) -> void
   {
      if(is_set_buffer)
      {
         if(std::stod(amount_buffer.c_str())<0)
            std::cerr << "Contrast value cannot be negative.\n";
         else
         {
            if(log_index != undo_redo_stack.size() && !is_revert)
               undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
            if(undo_redo_stack.size()<STACK_SIZE)
            {
               undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), std::stod(amount_buffer.c_str()), 'c');
               std::cout << undo_redo_stack.at(log_index-1);
            }
            else
            {
               switch(undo_redo_stack.front().action)
               {
                  case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
                  case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
                  case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
                  case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
                  case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
                  case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
                  case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               }
               std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
               undo_redo_stack.pop_back();
               undo_redo_stack.emplace(undo_redo_stack.end(), std::stod(amount_buffer.c_str()), 'c');
               std::cout << undo_redo_stack.at(log_index-1);
            }
            action_stack.emplace_back(std::stod(amount_buffer.c_str()), 'c');
            ycbcrData = image::RGBToYCbCr(rgbData);
            ycbcrData = image::contrast(ycbcrData, std::stod(amount_buffer.c_str()));
            rgbData = image::YCbCrToRGB(ycbcrData);
         }
         is_set_buffer=false;
         amount_buffer={};
      }
      else
      {
         if(log_index != undo_redo_stack.size() && !is_revert)
            undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
         if(undo_redo_stack.size()<STACK_SIZE)
         {
            undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 1.1, 'c');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         else
         {
            switch(undo_redo_stack.front().action)
            {
               case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
               case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
               case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            }
            std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
            undo_redo_stack.pop_back();
            undo_redo_stack.emplace(undo_redo_stack.end(), 1.1, 'c');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         action_stack.emplace_back(1.1, 'c');
         ycbcrData = image::RGBToYCbCr(rgbData);
         ycbcrData = image::contrast(ycbcrData, 1.1);
         rgbData = image::YCbCrToRGB(ycbcrData);
      }
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback exposure = [&](const SDL_Event& e) -> void
   {
      if(is_set_buffer)
      {
         if(log_index != undo_redo_stack.size() && !is_revert)
            undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
         if(undo_redo_stack.size()<STACK_SIZE)
         {
            undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), std::stod(amount_buffer.c_str()), 'e');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         else
         {
            switch(undo_redo_stack.front().action)
            {
               case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
               case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
               case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            }
            std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
            undo_redo_stack.pop_back();
            undo_redo_stack.emplace(undo_redo_stack.end(), std::stod(amount_buffer.c_str()), 'e');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         action_stack.emplace_back(std::stod(amount_buffer.c_str()), 'e');
         rgbData = image::exposure(rgbData, std::stod(amount_buffer.c_str()));
         is_set_buffer=false;
         amount_buffer={};
      }
      else
      {
         if(log_index != undo_redo_stack.size() && !is_revert)
            undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
         if(undo_redo_stack.size()<STACK_SIZE)
         {
            undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 0.1, 'e');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         else
         {
            switch(undo_redo_stack.front().action)
            {
               case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
               case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
               case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            }
            std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
            undo_redo_stack.pop_back();
            undo_redo_stack.emplace(undo_redo_stack.end(), 0.1, 'e');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         action_stack.emplace_back(0.1, 'e');
         rgbData = image::exposure(rgbData, 0.1);
      }
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback gaussianBlur = [&](const SDL_Event& e) -> void
   {
      if(is_set_buffer)
      {
         if(std::stod(amount_buffer.c_str())<0)
            std::cerr << "Gaussian Blur kernel size cannot be negative.\n";
         else
         {
            if(log_index != undo_redo_stack.size() && !is_revert)
               undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
            if(undo_redo_stack.size()<STACK_SIZE)
            {
               undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), std::stod(amount_buffer.c_str()), 'g');
               std::cout << undo_redo_stack.at(log_index-1);
            }
            else
            {
               switch(undo_redo_stack.front().action)
               {
                  case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
                  case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
                  case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
                  case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
                  case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
                  case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
                  case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               }
               std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
               undo_redo_stack.pop_back();
               undo_redo_stack.emplace(undo_redo_stack.end(), std::stod(amount_buffer.c_str()), 'g');
               std::cout << undo_redo_stack.at(log_index-1);
            }
            action_stack.emplace_back(std::stod(amount_buffer.c_str()), 'g');
            rgbData = image::gaussianBlur(rgbData, std::stod(amount_buffer.c_str()), args.width, args.height);
            is_set_buffer=false;
            amount_buffer={};
         }
      }
      else
      {
         if(log_index != undo_redo_stack.size() && !is_revert)
            undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
         if(undo_redo_stack.size()<STACK_SIZE)
         {
            undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 1, 'g');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         else
         {
            switch(undo_redo_stack.front().action)
            {
               case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
               case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
               case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            }
            std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
            undo_redo_stack.pop_back();
            undo_redo_stack.emplace(undo_redo_stack.end(), 1, 'g');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         action_stack.emplace_back(1, 'g');
         rgbData = image::gaussianBlur(rgbData, 1, args.width, args.height);
      }
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback saturation = [&](const SDL_Event& e) -> void
   {
      if(is_set_buffer)
      {
         if(std::stod(amount_buffer.c_str())<0)
            std::cerr << "Saturation value cannot be negative.\n";
         else
         {
            if(log_index != undo_redo_stack.size() && !is_revert)
               undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
            if(undo_redo_stack.size()<STACK_SIZE)
            {
               undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), std::stod(amount_buffer.c_str()), 's');
               std::cout << undo_redo_stack.at(log_index-1);
            }
            else
            {
               switch(undo_redo_stack.front().action)
               {
                  case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
                  case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
                  case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
                  case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
                  case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
                  case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
                  case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
                  case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               }
               std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
               undo_redo_stack.pop_back();
               undo_redo_stack.emplace(undo_redo_stack.end(), std::stod(amount_buffer.c_str()), 's');
               std::cout << undo_redo_stack.at(log_index-1);
            }
            action_stack.emplace_back(std::stod(amount_buffer.c_str()), 's');
            ycbcrData = image::RGBToYCbCr(rgbData);
            ycbcrData = image::saturation(ycbcrData, std::stod(amount_buffer.c_str()));
            rgbData = image::YCbCrToRGB(ycbcrData);
         }
         is_set_buffer=false;
         amount_buffer={};
      }
      else
      {
         if(log_index != undo_redo_stack.size() && !is_revert)
            undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
         if(undo_redo_stack.size()<STACK_SIZE)
         {
            undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 1.05, 's');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         else
         {
            switch(undo_redo_stack.front().action)
            {
               case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
               case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
               case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
               case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
               case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
               case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            }
            std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
            undo_redo_stack.pop_back();
            undo_redo_stack.emplace(undo_redo_stack.end(), 1.05, 's');
            std::cout << undo_redo_stack.at(log_index-1);
         }
         action_stack.emplace_back(1.05, 's');
         ycbcrData = image::RGBToYCbCr(rgbData);
         ycbcrData = image::saturation(ycbcrData, 1.05);
         rgbData = image::YCbCrToRGB(ycbcrData);
      }
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback extractRed = [&](const SDL_Event& e) -> void
   {
      if(log_index != undo_redo_stack.size() && !is_revert)
         undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
      if(undo_redo_stack.size()<STACK_SIZE)
      {
         undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 0, 'R');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      else
      {
         switch(undo_redo_stack.front().action)
         {
            case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
            case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
            case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
         }
         std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
         undo_redo_stack.pop_back();
         undo_redo_stack.emplace(undo_redo_stack.end(), 0, 'R');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      action_stack.emplace_back(0, 'R');;
      auto rgbData_data = image::extractR(rgbData).getData();
      for(auto& column : rgbData_data)
         column = {column.at(0), 0, 0};
      rgbData = rgbData_data;
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback extractGreen = [&](const SDL_Event& e) -> void
   {
      if(log_index != undo_redo_stack.size() && !is_revert)
         undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
      if(undo_redo_stack.size()<STACK_SIZE)
      {
         undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 0, 'G');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      else
      {
         switch(undo_redo_stack.front().action)
         {
            case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
            case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
            case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
         }
         std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
         undo_redo_stack.pop_back();
         undo_redo_stack.emplace(undo_redo_stack.end(), 0, 'G');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      action_stack.emplace_back(0, 'G');;
      auto rgbData_data = image::extractR(rgbData).getData();
      for(auto& column : rgbData_data)
         column = {0, column.at(0), 0};
      rgbData = rgbData_data;
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback extractBlue = [&](const SDL_Event& e) -> void
   {
      if(log_index != undo_redo_stack.size() && !is_revert)
         undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
      if(undo_redo_stack.size()<STACK_SIZE)
      {
         undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 0, 'B');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      else
      {
         switch(undo_redo_stack.front().action)
         {
            case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
            case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
            case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
         }
         std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
         undo_redo_stack.pop_back();
         undo_redo_stack.emplace(undo_redo_stack.end(), 0, 'B');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      action_stack.emplace_back(0, 'B');;
      auto rgbData_data = image::extractR(rgbData).getData();
      for(auto& column : rgbData_data)
         column = {0, 0, column.at(0)};
      rgbData = rgbData_data;
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback extractLuma = [&](const SDL_Event& e) -> void
   {
      if(log_index != undo_redo_stack.size() && !is_revert)
         undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
      if(undo_redo_stack.size()<STACK_SIZE)
      {
         undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 0, 'Y');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      else
      {
         switch(undo_redo_stack.front().action)
         {
            case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
            case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
            case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
         }
         std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
         undo_redo_stack.pop_back();
         undo_redo_stack.emplace(undo_redo_stack.end(), 0, 'Y');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      action_stack.emplace_back(0, 'Y');;
      auto rgbData_data = image::extractR(rgbData).getData();
      for(auto& column : rgbData_data)
         column = {column.at(0), column.at(0), column.at(0)};
      rgbData = rgbData_data;
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback extractChromaBlue = [&](const SDL_Event& e) -> void
   {
      if(log_index != undo_redo_stack.size() && !is_revert)
         undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
      if(undo_redo_stack.size()<STACK_SIZE)
      {
         undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 0, 'U');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      else
      {
         switch(undo_redo_stack.front().action)
         {
            case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
            case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
            case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
         }
         std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
         undo_redo_stack.pop_back();
         undo_redo_stack.emplace(undo_redo_stack.end(), 0, 'U');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      action_stack.emplace_back(0, 'U');;
      auto rgbData_data = image::extractR(rgbData).getData();
      for(auto& column : rgbData_data)
         if(column.at(0)>0.5)
            column = {0, 0, column.at(0)};
         else
            column = {1-column.at(0), 1-column.at(0), 0};
      rgbData = rgbData_data;
      pixelData = image::sdlPreprocess(rgbData);
   };

   Callback extractChromaRed = [&](const SDL_Event& e) -> void
   {
      if(log_index != undo_redo_stack.size() && !is_revert)
         undo_redo_stack.erase(undo_redo_stack.begin()+log_index, undo_redo_stack.end());
      if(undo_redo_stack.size()<STACK_SIZE)
      {
         undo_redo_stack.emplace(undo_redo_stack.begin()+(log_index++), 0, 'I');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      else
      {
         switch(undo_redo_stack.front().action)
         {
            case 'c': { rgbDataLog = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'e': { rgbDataLog = image::exposure(rgbDataLog, undo_redo_stack.front().intensity); break; }
            case 'g': { rgbDataLog = image::gaussianBlur(rgbDataLog, undo_redo_stack.front().intensity, args.width, args.height); break; }
            case 's': { rgbDataLog = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLog), undo_redo_stack.front().intensity)); break; }
            case 'R': { auto rgbDataLog_data = image::extractR(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), 0, 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'G': { auto rgbDataLog_data = image::extractG(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'B': { auto rgbDataLog_data = image::extractB(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {0, 0, column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'Y': { auto rgbDataLog_data = image::extractY(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
            case 'U': { auto rgbDataLog_data = image::extractCb(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLog = rgbDataLog_data; break; }
            case 'I': { auto rgbDataLog_data = image::extractCr(rgbDataLog).getData(); for(auto& column : rgbDataLog_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLog = rgbDataLog_data; break; }
         }
         std::shift_left(undo_redo_stack.begin(), undo_redo_stack.end(), 1);
         undo_redo_stack.pop_back();
         undo_redo_stack.emplace(undo_redo_stack.end(), 0, 'I');
         std::cout << undo_redo_stack.at(log_index-1);
      }
      action_stack.emplace_back(0, 'I');;
      auto rgbData_data = image::extractR(rgbData).getData();
      for(auto& column : rgbData_data)
         if(column.at(0)>0.5)
            column = {column.at(0), 0, 0};
         else
            column = {0, 1-column.at(0), 1-column.at(0)};
      rgbData = rgbData_data;
      pixelData = image::sdlPreprocess(rgbData);
   };

   while(!quit)
   {
      while(SDL_PollEvent(&e))
         if(e.type == SDL_QUIT)
            quit=true;
         else if(e.type == SDL_KEYDOWN)
            switch(e.key.keysym.sym)
            {
               case SDLK_a:
               case SDLK_d:
               case SDLK_f:
               case SDLK_h:
               case SDLK_j:
               case SDLK_k:
               case SDLK_n:
               case SDLK_o:
               case SDLK_p:
               case SDLK_t:
               case SDLK_v:
               case SDLK_w:
               case SDLK_x:
               case SDLK_UNDERSCORE:
               {
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  break;
               }
               case SDLK_b:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     gaussianBlur(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_c:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     contrast(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_e:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     exposure(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_s:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(SDL_GetModState() & SDL_Keymod::KMOD_GUI)
                  {
                     is_set_name=true;
                     updated_amount_buffer=false;
                  }
                  else
                     if(is_set_name)
                     {
                        name_buffer+=e.key.keysym.sym;
                        if(updated_name_buffer)
                           std::cout << "\r\033[K";
                        std::cout << "Name: " << name_buffer << std::flush;
                        updated_name_buffer=true;
                     }
                     else
                        saturation(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_r:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     extractRed(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_g:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     extractGreen(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_m:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     extractBlue(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_y:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     extractLuma(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_u:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     extractChromaBlue(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_i:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                     extractChromaRed(e);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_0:
               case SDLK_1:
               case SDLK_2:
               case SDLK_3:
               case SDLK_4:
               case SDLK_5:
               case SDLK_6:
               case SDLK_7:
               case SDLK_8:
               case SDLK_9:
               case SDLK_PERIOD:
               {
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                  {
                     is_set_buffer=true;
                     amount_buffer+=e.key.keysym.sym;
                     if(updated_amount_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Intensity: " << amount_buffer << std::flush;
                     updated_amount_buffer=true;
                  }
                  break;
               }
               case SDLK_MINUS:
               {
                  if(is_set_name)
                  {
                     name_buffer+=e.key.keysym.sym;
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                  }
                  else
                  {
                     if(amount_buffer == std::string{})
                     {
                        amount_buffer+=e.key.keysym.sym;
                        if(updated_amount_buffer)
                           std::cout << "\r\033[K";
                        std::cout << "Intensity: " << amount_buffer << std::flush;
                     }
                     else
                     {
                        if(updated_amount_buffer)
                           std::cout << "\r\033[K" << std::flush;
                        std::cerr << "Invalid number for intensity.\n";
                        is_set_buffer=false;
                        amount_buffer={};
                     }
                     updated_amount_buffer=true;
                  }
                  break;
               }
               case SDLK_BACKSPACE:
               {
                  if(is_set_name)
                  {
                     if(name_buffer!=std::string{})
                        name_buffer.resize(name_buffer.size()-1);
                     if(!name_buffer.size())
                     {
                        std::cout << "\r\033[K" << std::flush;
                        updated_name_buffer=false;
                     }
                     else
                        std::cout << "\r\033[K" << "Name: " << name_buffer << std::flush;
                  }
                  else
                  {
                     if(amount_buffer!=std::string{})
                        amount_buffer.resize(amount_buffer.size()-1);
                     if(updated_amount_buffer)
                     {
                        if(!amount_buffer.size())
                        {
                           is_set_buffer=false;
                           std::cout << "\r\033[K" << std::flush;
                        }
                        else
                           std::cout << "\r\033[KIntensity: " << amount_buffer << std::flush;
                     }
                     else
                     {
                        if(!amount_buffer.size())
                        {
                           is_set_buffer=false;
                           std::cout << "\r\033[K" << std::flush;
                        }
                        else
                           std::cout << "Intensity: " << amount_buffer << std::flush;
                     }
                     updated_amount_buffer=true;
                  }
                  break;
               }
               case SDLK_RETURN:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     if(name_buffer.contains(".ppm"))
                        name_buffer.resize(name_buffer.size()-4);
                     std::ofstream saved_file(project_root+"/out/"+name_buffer+".ppm");
                     image::ppmHeader(saved_file, args);
                     image::ppmData(saved_file, rgbData, args);
                     std::cout << std::endl << "Saved image: " << name_buffer << ".ppm" << std::endl;
                     name_buffer_prev=name_buffer;
                     name_buffer={};
                     is_set_name=false;
                     updated_name_buffer=false;
                  }
                  else
                     switch(undo_redo_stack.back().action)
                     {
                        case 'c': { amount_buffer=std::to_string(undo_redo_stack.back().intensity); is_set_buffer=true; contrast(e); break; }
                        case 'e': { amount_buffer=std::to_string(undo_redo_stack.back().intensity); is_set_buffer=true; exposure(e); break; }
                        case 'g': { amount_buffer=std::to_string(undo_redo_stack.back().intensity); is_set_buffer=true; gaussianBlur(e); break; }
                        case 's': { amount_buffer=std::to_string(undo_redo_stack.back().intensity); is_set_buffer=true; saturation(e); break; }
                     }
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_z:
               {
                  if(SDL_GetModState() & SDL_Keymod::KMOD_SHIFT && SDL_GetModState() & SDL_Keymod::KMOD_GUI)
                  {
                     if(log_index != undo_redo_stack.size())
                     {
                        rgbDataLogCopy = rgbData;
                        is_revert=true;
                        switch(undo_redo_stack.at(log_index).action)
                        {
                           case 'c': { rgbDataLogCopy = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLogCopy), undo_redo_stack.at(log_index).intensity)); break; }
                           case 'e': { rgbDataLogCopy = image::exposure(rgbDataLogCopy, undo_redo_stack.at(log_index).intensity); break; }
                           case 'g': { rgbDataLogCopy = image::gaussianBlur(rgbDataLogCopy, undo_redo_stack.at(log_index).intensity, args.width, args.height); break; }
                           case 's': { rgbDataLogCopy = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLogCopy), undo_redo_stack.at(log_index).intensity)); break; }
                           case 'R': { auto rgbDataLogCopy_data = image::extractR(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {column.at(0), 0, 0}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'G': { auto rgbDataLogCopy_data = image::extractG(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {0, column.at(0), 0}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'B': { auto rgbDataLogCopy_data = image::extractB(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {0, 0, column.at(0)}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'Y': { auto rgbDataLogCopy_data = image::extractY(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'U': { auto rgbDataLogCopy_data = image::extractCb(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'I': { auto rgbDataLogCopy_data = image::extractCr(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                        }
                        action_stack.emplace_back(0, 'r');
                        is_revert=false;
                        log_index++;
                        rgbData = rgbDataLogCopy;
                        pixelData = image::sdlPreprocess(rgbData);
                        if(updated_amount_buffer)
                           std::cout << std::endl;
                        std::cout << "Reapplied the last action.\n";
                     }
                  }
                  else if(SDL_GetModState() & SDL_Keymod::KMOD_GUI && log_index)
                  {
                     rgbDataLogCopy = rgbDataLog;
                     log_index--;
                     is_revert=true;
                     for(u16 i{}; i<log_index; i++)
                     {
                        switch(undo_redo_stack.at(i).action)
                        {
                           case 'c': { rgbDataLogCopy = image::YCbCrToRGB(image::contrast(image::RGBToYCbCr(rgbDataLogCopy), undo_redo_stack.at(i).intensity)); break; }
                           case 'e': { rgbDataLogCopy = image::exposure(rgbDataLogCopy, undo_redo_stack.at(i).intensity); break; }
                           case 'g': { rgbDataLogCopy = image::gaussianBlur(rgbDataLogCopy, undo_redo_stack.at(i).intensity, args.width, args.height); break; }
                           case 's': { rgbDataLogCopy = image::YCbCrToRGB(image::saturation(image::RGBToYCbCr(rgbDataLogCopy), undo_redo_stack.at(i).intensity)); break; }
                           case 'R': { auto rgbDataLogCopy_data = image::extractR(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {column.at(0), 0, 0}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'G': { auto rgbDataLogCopy_data = image::extractG(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {0, column.at(0), 0}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'B': { auto rgbDataLogCopy_data = image::extractB(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {0, 0, column.at(0)}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'Y': { auto rgbDataLogCopy_data = image::extractY(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) column = {column.at(0), column.at(0), column.at(0)}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'U': { auto rgbDataLogCopy_data = image::extractCb(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) if(column.at(0)>0.5) column = {0, 0, column.at(0)}; else column = {1-column.at(0), 1-column.at(0), 0}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                           case 'I': { auto rgbDataLogCopy_data = image::extractCr(rgbDataLogCopy).getData(); for(auto& column : rgbDataLogCopy_data) if(column.at(0)>0.5) column = {column.at(0), 0, 0}; else column = {0, 1-column.at(0), 1-column.at(0)}; rgbDataLogCopy = rgbDataLogCopy_data; break; }
                        }
                     }
                     action_stack.emplace_back(0, 'u');
                     is_revert=false;
                     rgbData = rgbDataLogCopy;
                     pixelData = image::sdlPreprocess(rgbData);
                     if(updated_amount_buffer)
                        std::cout << std::endl;
                     std::cout << "Reverted the last action.\n";
                  }
                  else
                     if(is_set_name)
                     {
                        if(updated_name_buffer)
                           std::cout << "\r\033[K";
                        std::cout << "Name: " << name_buffer << std::flush;
                        updated_name_buffer=true;
                        name_buffer+=e.key.keysym.sym;
                     }
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_l:
               {
                  if(updated_amount_buffer)
                     std::cout << std::endl;
                  if(is_set_name)
                  {
                     if(updated_name_buffer)
                        std::cout << "\r\033[K";
                     std::cout << "Name: " << name_buffer << std::flush;
                     updated_name_buffer=true;
                     name_buffer+=e.key.keysym.sym;
                  }
                  else
                     printLog(undo_redo_stack, log_index);
                  updated_amount_buffer=false;
                  break;
               }
               case SDLK_q:
               {
                  if(SDL_GetModState() & SDL_Keymod::KMOD_GUI)
                  {
                     if(name_buffer_prev.contains(".ppm"))
                        name_buffer_prev.resize(name_buffer_prev.size()-4);
                     std::ofstream action_log(project_root+"/logs/"+name_buffer_prev+"-actions.log");
                     writeLog(action_log, action_stack);
                     quit=true;
                  }
                  else
                     if(is_set_name)
                     {
                        if(updated_name_buffer)
                           std::cout << "\r\033[K";
                        std::cout << "Name: " << name_buffer << std::flush;
                        updated_name_buffer=true;
                        name_buffer+=e.key.keysym.sym;
                     }
                  break;
               }
            }
      SDL_UpdateTexture(texture, nullptr, pixelData.data(), 3*args.width);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, nullptr, nullptr);
      SDL_RenderPresent(renderer);
   }

   if(updated_amount_buffer || updated_name_buffer)
      std::cout << std::endl;
   std::cout << "Quitting the image viewer..." << std::endl;
   SDL_DestroyTexture(texture);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   return 0;
}
