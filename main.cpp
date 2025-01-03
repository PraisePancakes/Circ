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
    std::initializer_list<std::string> key_path = {"rest"};
    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
    //SERIALIZATION check cfg file before and after setting
    auto d = cfgl.CFGAttr<std::string>(key_path);
    cfgl.CFGAttrSet<std::string>(key_path, std::string("jest"));
    

  

    return 0;
}
