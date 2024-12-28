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
   
    auto v = cfgl.CFGAttr<double>({ "test" });
    
    std::cout << v;
 
   
   

    return 0;
}
