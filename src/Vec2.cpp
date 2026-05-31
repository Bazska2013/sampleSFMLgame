//
// Created by Balázs Takács on 2026. 05. 30..
//

#include "Vec2.h"

#include <cmath>

Vec2::Vec2() = default;
Vec2::Vec2(const float xin, const float yin) : x(xin), y(yin) {};

bool Vec2::operator == (const Vec2 & rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2 & rhs) const {
    return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator + (const Vec2 & rhs) const {
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator - (const Vec2 & rhs) const {
    return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator * (const Vec2 & rhs) const {
    return {x * rhs.x, y * rhs.y};
}

Vec2 Vec2::operator / (const Vec2 & rhs) const {
    if (rhs.x != 0 || rhs.y != 0) {
        return {x / rhs.x, y / rhs.y};
    }
    return *this;
}

void Vec2::operator += (const Vec2 & rhs) {
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator -= (const Vec2 & rhs) {
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator *= (const Vec2 & rhs) {
    x *= rhs.x;
    y *= rhs.y;
}

void Vec2::operator /= (const Vec2 & rhs) {
    if (rhs.x != 0 || rhs.y != 0) {
        x /= rhs.x;
        y /= rhs.y;
    }
}

float Vec2::dist(const Vec2 & rhs) const {
    return std::hypot(x - rhs.x, y - rhs.y);
}

void Vec2::normalize() {
    if (const float len = length(); len != 0) {
        x /= len;
        y /= len;
    } else {
        x = 0;
        y = 0;
    }
}

float Vec2::length() const {
    return std::hypot(x, y);
}
