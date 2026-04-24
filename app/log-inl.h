#ifndef PROIMG_APP_LOG_H_
#define PROIMG_APP_LOG_H_

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

#include "math/types.h"

void fillBuffer()
{
   struct winsize w;
   ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
   std::string filler(w.ws_col, '=');
   std::cout << filler << std::endl;
}

struct actionLog
{
   public:
      double intensity;
      u8 action;

      friend std::ostream& operator<<(std::ostream& output, const actionLog& log)
      {
         switch(log.action)
         {
            case 'c': { output << "Applying effect " << "[Contrast] with intensity [" << log.intensity << "]"; break; }
            case 'e': { output << "Applying effect " << "[Exposure] with intensity [" << log.intensity << "]"; break; }
            case 'g': { output << "Applying effect " << "[Gaussian Blur] with intensity [" << log.intensity << "]"; break; }
            case 'r': { output << "[Redo]"; break; }
            case 's': { output << "Applying effect " << "[Saturation] with intensity [" << log.intensity << "]"; break; }
            case 'u': { output << "[Undo]"; break; }
            case 'R': { output << "[Extract Red]"; break; }
            case 'G': { output << "[Extract Green]"; break; }
            case 'B': { output << "[Extract Blue]"; break; }
            case 'Y': { output << "[Extract Luma]"; break; }
            case 'U': { output << "[Extract Chroma Blue]"; break; }
            case 'I': { output << "[Extract Chroma Red]"; break; }
         }
         output << std::endl;
         return output;
      }
};

void printLog(const std::vector<actionLog>& log_stack, u16 index)
{
   fillBuffer();
   std::cout << "Log size: " << log_stack.size() << std::endl;
   for(u16 i{}; i<log_stack.size(); i++)
   {
      if(i==index)
         std::cout << "->";
      std::cout << log_stack.at(i);
   }
   fillBuffer();
}

void writeLog(std::ostream& output, const std::vector<actionLog>& log_stack)
{
   output << "Total actions: " << log_stack.size() << std::endl;
   for(const auto& log : log_stack)
      output << log;
}

#endif
