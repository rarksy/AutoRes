#pragma once

struct vec2
{
    int x;
    int y;

    vec2() : x(0), y(0) {}

    vec2(int _x, int _y)
    {
        this->x = _x;
        this->y = _y;
    }

    vec2 operator+(const vec2& other) const
    {
        return vec2(x + other.x, y + other.y);
    }
};