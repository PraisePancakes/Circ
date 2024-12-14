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
  
    auto clear_screen = CFGAttr<double>("background_color", cfgl.interp);
    CircInitWindow(cfgl.interp);
    

    while (!WindowShouldClose()) {
        ClearBackground((Color)clear_screen);
        BeginDrawing();
        EndDrawing();
    }

    return 0;
}
