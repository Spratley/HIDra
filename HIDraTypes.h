#pragma once

#include "Defs/HIDraDefs.h"

#include <cstring> // For Memset
#include <utility> // For std::move
#if HIDra_Debug
#include <cstdio>
#endif // HIDra_Debug

namespace HIDra
{
    using HIDra_UInt32 = unsigned int;
    using HIDra_UInt16 = unsigned short;
    using HIDra_UInt8 = unsigned char;

#if HIDra_Gamepad
    struct GamepadID
    {
        // If there's more than 256 gamepads connected you've got bigger problems than ID collisions
        using StorageType = HIDra_UInt8;
        static constexpr StorageType InvalidGamepadID = 0xFF;

        constexpr GamepadID(StorageType id)
            : m_id(id)
        {}
        constexpr operator StorageType() { return m_id; }

        constexpr bool IsValid() const { return m_id != InvalidGamepadID; }

        constexpr bool operator==(GamepadID const& rhs) { return m_id == rhs.m_id; }
        constexpr bool operator!=(GamepadID const& rhs) { return m_id != rhs.m_id; }

    private:
        StorageType m_id = InvalidGamepadID;
    };

    using Vendor = HIDra_UInt16;
    using Product = HIDra_UInt16;
#endif // HIDra_Gamepad

    struct Vec2f
    {
        static const Vec2f s_zero;

        constexpr Vec2f() = default;
        constexpr Vec2f(float x, float y)
            : m_x(x)
            , m_y(y)
        {}

        // Used for averaging
        friend Vec2f operator+(Vec2f const& lhs, Vec2f const& rhs)
        {
            return Vec2f(lhs.m_x + rhs.m_x, lhs.m_y + rhs.m_y);
        }
        friend Vec2f operator*(Vec2f const& vec, HIDra_UInt32 scalar)
        {
            return Vec2f(vec.m_x * scalar, vec.m_y * scalar);
        }
        friend Vec2f operator/(Vec2f const& vec, HIDra_UInt32 scalar)
        {
            return Vec2f(vec.m_x / scalar, vec.m_y / scalar);
        }

        float m_x = 0.0f;
        float m_y = 0.0f;
    };

    // Helper for Keyboard
    // Bitfield for large amounts of data (i.e. a keyboard with 256 possible keycodes)
    template <HIDra_UInt32 ByteCount>
    struct BigBitfield
    {
    public:
        bool GetBit(HIDra_UInt32 bitIndex) const;
        void SetBit(HIDra_UInt32 bitIndex, bool value);

        void ZeroMemory();

    private:
        bool GetPagedIndex(HIDra_UInt32 bitIndex, HIDra_UInt32& outPageIndex, HIDra_UInt32& outPagedBitIndex) const;

    private:
        HIDra_UInt8 m_dataPages[ByteCount] = {};
    };

    template <HIDra_UInt32 ByteCount>
    inline bool BigBitfield<ByteCount>::GetBit(HIDra_UInt32 bitIndex) const
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

    template <HIDra_UInt32 ByteCount>
    inline void BigBitfield<ByteCount>::SetBit(HIDra_UInt32 bitIndex, bool value)
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

    template <HIDra_UInt32 ByteCount>
    inline void BigBitfield<ByteCount>::ZeroMemory()
    {
        memset(m_dataPages, 0, sizeof(m_dataPages));
    }

    template <HIDra_UInt32 ByteCount>
    inline bool BigBitfield<ByteCount>::GetPagedIndex(HIDra_UInt32 bitIndex,
                                                      HIDra_UInt32& outPageIndex,
                                                      HIDra_UInt32& outPagedBitIndex) const
    {
#if HIDra_Debug
        constexpr HIDra_UInt32 maxBitIndex = ByteCount * 8;
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
} // namespace HIDra