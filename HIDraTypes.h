#pragma once

#include "HIDraDefs.h"

#include <cstring> // For Memset
#if HIDra_Debug
#include <cstdio>
#endif // HIDra_Debug

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

    // Helper for Keyboard
    // Bitfield for large amounts of data (i.e. a keyboard with 256 possible keycodes)
    template<HIDra_UInt32 BytesCount>
    struct BigBitfield
    {
    public:
        bool GetBit(HIDra_UInt32 bitIndex) const;
        void SetBit(HIDra_UInt32 bitIndex, bool value);

        void ZeroMemory();

    private:
        bool GetPagedIndex(HIDra_UInt32 bitIndex, HIDra_UInt32& outPageIndex, HIDra_UInt32& outPagedBitIndex) const;

    private:
        HIDra_UInt8 m_dataPages[BytesCount] = {};
    };

    template<HIDra_UInt32 BytesCount>
    inline bool BigBitfield<BytesCount>::GetBit(HIDra_UInt32 bitIndex) const
    {
        HIDra_UInt32 pageIndex;
        HIDra_UInt32 pagedBitIndex;
        if (!GetPagedIndex(bitIndex, pageIndex, pagedBitIndex))
        {
#if HIDra_Debug
            // Out of bounds
            return false;
#endif // HIDra_Debug
        }

        return (m_dataPages[pageIndex] & (0b1 << pagedBitIndex)) > 0;
    }
    
    template<HIDra_UInt32 BytesCount>
    inline void BigBitfield<BytesCount>::SetBit(HIDra_UInt32 bitIndex, bool value)
    {
        HIDra_UInt32 pageIndex;
        HIDra_UInt32 pagedBitIndex;
        if (!GetPagedIndex(bitIndex, pageIndex, pagedBitIndex))
        {
#if HIDra_Debug
            // Out of bounds
            return;
#endif // HIDra_Debug
        }

        HIDra_UInt8 mask = 0b1 << pagedBitIndex;
        m_dataPages[pageIndex] = (m_dataPages[pageIndex] & ~mask) | (static_cast<HIDra_UInt8>(value) << pagedBitIndex);
    }

    template<HIDra_UInt32 BytesCount>
    inline void BigBitfield<BytesCount>::ZeroMemory()
    {
        memset(m_dataPages, 0, sizeof(m_dataPages));
    }

    template<HIDra_UInt32 BytesCount>
    inline bool BigBitfield<BytesCount>::GetPagedIndex(HIDra_UInt32 bitIndex, HIDra_UInt32& outPageIndex, HIDra_UInt32& outPagedBitIndex) const
    {
#if HIDra_Debug
        constexpr HIDra_UInt32 maxBitIndex = BytesCount * 8;
        if (bitIndex >= maxBitIndex)
        {
            printf("Attempting to access a BigBitfield out of bounds!");
            return false;
        }
#endif // HIDra_Debug

        outPageIndex = bitIndex >> 3; // Divide by 8 with truncate
        outPagedBitIndex = bitIndex & 0b0111; // Get truncated bits as remainder
        return true;
    }
}