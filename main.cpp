#include <iostream>
#include <vector>
#include <cmath>
#include "Circ/include/CFGLoader.hpp"

const std::string cfg_path = "cfg/config.circ";

/*
    TO DO ARCHIVE FORMATTER
*/

int main() {
    std::initializer_list<std::string> key_path = { "a", "t" };
    Circ::CFGLoader cfgl(cfg_path);
    cfgl.CFGAttrCreate(key_path, 6);
    cfgl.CFGAttrCreate({ "a", "b" }, 2);
    cfgl.Serialize();
    std::cout << cfgl.CFGAttr<int>({ "a", "b" });


    return 0;
}