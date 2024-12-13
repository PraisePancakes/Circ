#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>
#include "Circ/include/PointPlotter.hpp"

const std::string title = "Circ";
const std::string cfg_path = "cfg/config.txt";

void CustomLog(int msgType, const char* text, va_list args)
{
    return;
}


int main() {
    SetTraceLogCallback(CustomLog);
    PointPlotter p(cfg_path);
    p.run();
    return 0;
}
