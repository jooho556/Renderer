#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

namespace Angle
{
    const float PI = 4.0f*atan(1.0f);
    inline float DegToRad(float degree) { return 2 * PI * degree / 360; }
    inline float RadToDeg(float rad) { return rad * 180 / PI; }
}

class Ellipse
{
public:
    Ellipse(float a, float eccen)
        : radius_a(a), eccentricity(eccen) {}

    glm::vec3 GetPoint(float t) const
    {
        float x = radius_a * std::cos(t * 2 * Angle::PI);
        float y = radius_a * eccentricity * std::sin(t * 2 * Angle::PI);
        return glm::vec4(x, y, 0, 1);
    }

    float GetRadA() const { return radius_a; }
    float GetRadB() const { return radius_a * eccentricity; }

    void AddRadius(float a)
    {
        radius_a += a;
    }

private:
    float radius_a = 1;
    float eccentricity = 1;
};