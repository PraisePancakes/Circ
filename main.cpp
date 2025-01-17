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
    TO DO ARCHIVE FORMATTER
*/

int main() {
    std::initializer_list<std::string> key_path = {"a", "t"};
    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
    cfgl.CFGAttrSet(key_path, 1);
    cfgl.Serialize();
   
   
    return 0;
}
