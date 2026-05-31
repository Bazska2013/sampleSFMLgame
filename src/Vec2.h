//
// Created by Balázs Takács on 2026. 05. 30..
//

#ifndef CMAKESFMLPROJECT_VEC2_H
#define CMAKESFMLPROJECT_VEC2_H


class Vec2 {
public:
    float x = 0, y = 0;

    Vec2();
    Vec2(float xin, float yin);

    bool operator == (const Vec2& rhs) const;
    bool operator != (const Vec2& rhs) const;

    Vec2 operator + (const Vec2& rhs) const;
    Vec2 operator - (const Vec2& rhs) const;
    Vec2 operator * (const Vec2& rhs) const;
    Vec2 operator / (const Vec2& rhs) const;

    void operator += (const Vec2& rhs);
    void operator -= (const Vec2& rhs);
    void operator *= (const Vec2& rhs);
    void operator /= (const Vec2& rhs);

    [[nodiscard]] float dist(const Vec2& rhs) const;
    void normalize();
    [[nodiscard]] float length() const;
};


#endif //CMAKESFMLPROJECT_VEC2_H
