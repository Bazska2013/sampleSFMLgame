//
// Created by Balázs Takács on 2026. 05. 30..
//

#ifndef CMAKESFMLPROJECT_COMPONENTS_H
#define CMAKESFMLPROJECT_COMPONENTS_H
#include "Vec2.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Glsl.hpp"


class CTransform {
public:
    Vec2 pos = { 0.0f, 0.0f };
    Vec2 velocity = { 0.0f, 0.0f };
    float angle = 0;

    CTransform(const Vec2 & p, const Vec2 & v, float a) : pos(p), velocity(v), angle(a) {}
};

class CShape {
public:
    sf::CircleShape circle;

    CShape(float radius, int points, const sf::Color & fill, const sf::Color & outline, float thickness) : circle(radius, points) {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin({radius, radius});
    }
};

class CCollision {
public:
    float radius = 0;
    explicit CCollision(const float r) : radius(r) {}
};

class CScore {
public:
    int score = 0;
    explicit CScore(const int s) : score(s) {}
};

class CLifespan {
public:
    int remaining = 0;
    int total = 0;
    explicit CLifespan(const int t) : remaining(t), total(t) {}
};

class CInput {
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool shoot = false;
    
    CInput() = default;
};

#endif //CMAKESFMLPROJECT_COMPONENTS_H
