#pragma once
#include <raylib.h>
#include "Point.hpp"
#include <iostream>
#include <vector>
#include "CFGINTERP/interp.hpp"


template<typename WrapperType>
WrapperType CFGAttr(const std::string& key, CircCFGInterp::Interpreter* interp) {
    return std::any_cast<WrapperType>(interp->Get(key));
};

//std::string title = CFGAttr<std::string>("window_title");

class PointPlotter {
    std::vector<Point> plotted_points;
    int width;
    int height;
    CircCFGInterp::Interpreter* interp;
public:
    PointPlotter(const std::string& cfg) {
        interp = new CircCFGInterp::Interpreter(cfg);
        auto title = CFGAttr<std::string>("window_title", interp);
        auto width = CFGAttr<double>("window_width", interp);
        auto height = CFGAttr<double>("window_height", interp);
        InitWindow(width, height, title.data());
    };

    

    void run() {
        double clear_screen = CFGAttr<double>("background_color", interp);
        Color c = (Color)clear_screen;
        while (!WindowShouldClose()) {
            ClearBackground(c);
            BeginDrawing();
            EndDrawing();
        }
    
    };

    
    
    ~PointPlotter() {};
};
