#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>
#include "Circ/include/CFGLoader.hpp"

const std::string cfg_path = "cfg/config.circ";

void CustomLog(int msgType, const char* text, va_list args)
{
    return;
}
/*
    TO DO ARCHIVE (SERIALIZATION AND DESERIALIZATION)
*/

int main() {
    std::initializer_list<std::string> key_path = {"best", "rest"};
    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
 
    cfgl.CFGAttrSet<int>(key_path, 7);

    int x = cfgl.CFGAttr<int>(key_path);
    std::cout << x;
    
    //SERIALIZATION check cfg file before and after setting
    cfgl.Serialize();
  

  

    return 0;
}
