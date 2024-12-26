#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>
#include "Circ/include/CFGLoader.hpp"

const std::string cfg_path = "cfg/config.txt";

void CustomLog(int msgType, const char* text, va_list args)
{
    return;
}


int main() {

    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
   
    auto v = cfgl.CFGAttr<std::vector<std::any>>({ "arr" });
    auto iv = std::any_cast<std::vector<std::any>>(v[1]);
    auto d = std::any_cast<double>(iv[0]);
    std::cout << d;
 
   
   

    return 0;
}
