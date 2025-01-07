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
    std::initializer_list<std::string> key_path = {"test"};
    SetTraceLogCallback(CustomLog);
    Circ::CFGLoader cfgl(cfg_path);
    //SERIALIZATION check cfg file before and after setting
   
    std::vector <std::any> v{ std::vector<std::any>{1}, std::vector<std::any>{0}, std::vector<std::any>{4, 2} };
    cfgl.CFGAttrSet <std::vector<std::any>> (key_path,v);
    cfgl.Serialize();

  

    return 0;
}
