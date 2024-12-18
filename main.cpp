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
  
    auto clear_screen = cfgl.CFGAttr<double>("background_color");
    auto ub = cfgl.CFGAttr<bool>("unary_bool");
    auto ud = cfgl.CFGAttr<double>("unary_double");
    std::cout << "unary_bool = " << ub << std::endl;
    std::cout << "unary_double = " << ud << std::endl;
    auto m = cfgl.CFGObj({ "object" , "pos"});
    std::cout << "object { pos { x } } = " << std::any_cast<double>(m.at("x"));

    cfgl.CircInitWindow();
    

    while (!WindowShouldClose()) {
        ClearBackground((Color)clear_screen);
        BeginDrawing();
        EndDrawing();
    }

    return 0;
}
