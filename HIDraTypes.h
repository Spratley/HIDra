#pragma once

namespace HIDra
{
#if HIDra_Gamepad
    using Vendor = unsigned short;
    using Product = unsigned short;
#endif // HIDra_Gamepad

    using HIDra_UInt32 = unsigned int;
    using HIDra_UInt16 = unsigned short;
    using HIDra_UInt8 = unsigned char;

    struct Vec2f
    {
        static const Vec2f s_zero;

        Vec2f() = default;
        Vec2f(float x, float y) : m_x(x), m_y(y) {}

        // Used for averaging
        friend Vec2f operator+(Vec2f const& lhs, Vec2f const& rhs) { return Vec2f(lhs.m_x + rhs.m_x, lhs.m_y + rhs.m_y); }
        friend Vec2f operator*(Vec2f const& vec, HIDra_UInt32 scalar) { return Vec2f(vec.m_x * scalar, vec.m_y * scalar); }
        friend Vec2f operator/(Vec2f const& vec, HIDra_UInt32 scalar) { return Vec2f(vec.m_x / scalar, vec.m_y / scalar); }
        
        float m_x = 0.0f;
        float m_y = 0.0f;
    };
}