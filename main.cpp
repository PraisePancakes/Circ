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

    /*
        implement :
        auto v = cfgl.CFGAttr<std::vector<double>>({"arr"});
        auto d = v[0];

    */
    auto v = cfgl.CFGAttr<std::vector<double>>({ "arr" });
    auto d = v[1];

    std::cout << d;
 
   
   

    return 0;
}
