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


int main() {

    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
    double d = cfgl.CFGAttr<double>({ "rest",  "x"});
    auto v = cfgl.CFGAttr<std::vector<std::any>>({ "rest", "arr" });
    std::cout << std::any_cast<double>(v[2]);

  

    return 0;
}
