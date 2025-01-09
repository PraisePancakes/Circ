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
    std::initializer_list<std::string> key_path = {"test"};
    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
    int x = cfgl.CFGAttr<int>("test");
    cfgl.CFGAttrSet<int>({ "test" }, 4);
    cfgl.Serialize();
    //SERIALIZATION check cfg file before and after setting
   
  

  

    return 0;
}
