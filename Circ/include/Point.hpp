#pragma once
#include <raylib.h>

struct Point {
    Vector2 pos;
    Point() : pos({ 0, 0 }) {};
    Point(float x, float y) : pos({ x , y }) {};
    ~Point() = default;
};