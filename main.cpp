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

    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
    double d = cfgl.CFGAttr<double>({ "rest",  "x"});
    cfgl.CFGAttrSet<double>({"rest", "x"}, (double)4);
    std::cout << cfgl.CFGAttr<double>({ "rest", "x" });

  

    return 0;
}
