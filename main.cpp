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
    auto w = CFGAttr<double>("window_width", cfgl.interp);
    auto h = CFGAttr<double>("window_height", cfgl.interp);
    auto t = CFGAttr<std::string>("window_title", cfgl.interp);

    auto clear_screen = CFGAttr<double>("background_color", cfgl.interp);

    InitWindow(w,h,t.data());

    while (!WindowShouldClose()) {
        ClearBackground((Color)clear_screen);
        BeginDrawing();
        EndDrawing();
    }

    return 0;
}
