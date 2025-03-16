#pragma once
#include <cmath>

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    // Basic vector operations
    Vec2 operator+(const Vec2& other) const { return { x + other.x, y + other.y }; }
    Vec2 operator-(const Vec2& other) const { return { x - other.x, y - other.y }; }
    Vec2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }

    float length() const { return std::sqrt(x * x + y * y); }
    float length_sq() const { return x * x + y * y; }
};