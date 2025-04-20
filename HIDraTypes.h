#pragma once

namespace HIDra
{
    using Vendor = unsigned short;
    using Product = unsigned short;

    using HIDra_UInt32 = unsigned int;
    using HIDra_UInt16 = unsigned short;
    using HIDra_UInt8 = unsigned char;

    struct Vec2f
    {
        float m_x = 0.0f;
        float m_y = 0.0f;
    };
}