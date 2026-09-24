/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__SOURCE__IO_H
#define ARX__SOURCE__IO_H

#include <bit>
#include <cstdint>
#include <istream>

namespace ARX
{
    template<std::integral ValueType>
    [[nodiscard]]
    inline auto ToNetworkByteOrder(ValueType Value) -> ValueType
    {
        if constexpr(std::endian::native == std::endian::little)
        {
            return std::byteswap(Value);
        }
        else
        {
            return Value;
        }
    }
    
    template<std::integral ValueType>
    [[nodiscard]]
    inline auto FromNetworkByteOrder(ValueType Value) -> ValueType
    {
        if constexpr(std::endian::native == std::endian::little)
        {
            return std::byteswap(Value);
        }
        else
        {
            return Value;
        }
    }
    
    [[nodiscard]]
    inline static auto ReadStringWithTermination(std::istream & IStream) -> std::string
    {
        auto Result = std::string{};
        auto Char = '\0';
        
        while(IStream.get(Char))
        {
            if(Char != '\0')
            {
                Result += Char;
            }
            else
            {
                break;
            }
        }
        
        return Result;
    }
    
    [[nodiscard]]
    inline auto ReadUnsignedInteger8Bit(std::istream & IStream) -> std::uint8_t
    {
        auto Result = std::uint8_t{};
        
        IStream.read(reinterpret_cast<std::istream::char_type *>(&Result), sizeof(Result));
        
        return ARX::FromNetworkByteOrder(Result);
    }
    
    [[nodiscard]]
    inline auto ReadUnsignedInteger32Bit(std::istream & IStream) -> std::uint32_t
    {
        auto Result = std::uint32_t{};
        
        IStream.read(reinterpret_cast<std::istream::char_type *>(&Result), sizeof(Result));
        
        return ARX::FromNetworkByteOrder(Result);
    }
    
    inline auto WriteUnsignedInteger8Bit(std::ostream & OStream, std::uint8_t Value) -> void
    {
        Value = ARX::ToNetworkByteOrder(Value);
		OStream.write(reinterpret_cast<std::ostream::char_type *>(&Value), sizeof(Value));
    }
    
    inline auto WriteUnsignedInteger32Bit(std::ostream & OStream, std::uint32_t Value) -> void
    {
        Value = ARX::ToNetworkByteOrder(Value);
		OStream.write(reinterpret_cast<std::ostream::char_type *>(&Value), sizeof(Value));
    }
}

#endif
