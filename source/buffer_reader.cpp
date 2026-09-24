/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <arx/buffer.h>
#include <arx/buffer_reader.h>

ARX::BufferReader::BufferReader(ARX::Buffer const & Buffer) :
	m_Buffer{Buffer},
	m_Marker{Buffer, 0, ARX::Buffer::Marker::Alignment::Left}
{
}

ARX::BufferReader::BufferReader(ARX::Buffer const & Buffer, ARX::Buffer::size_type Position) :
	m_Buffer{Buffer},
	m_Marker{Buffer, Position, ARX::Buffer::Marker::Alignment::Left}
{
}

ARX::BufferReader::BufferReader(ARX::BufferReader const & Other) :
    ARX::BufferReader::BufferReader{Other.m_Buffer, Other.GetPosition()}
{
}

auto ARX::BufferReader::Read(ARX::Buffer::size_type Length, ARX::Buffer::pointer Buffer) -> ARX::Buffer::size_type
{
	if(m_Marker.IsValid() == false)
	{
		throw std::runtime_error{"ARX::BufferWriter::Write: Buffer was destroyed."};
	}
	if(m_Marker.GetPosition() + Length > m_Buffer.GetLength())
	{
		throw std::out_of_range{"Trying to read after the end."};
	}
	std::copy(m_Buffer.GetBegin() + m_Marker.GetPosition(), m_Buffer.GetBegin() + m_Marker.GetPosition() + Length, Buffer);
	m_Marker.SetPosition(m_Marker.GetPosition() + Length);
	
	return Length;
}

auto ARX::BufferReader::GetBuffer() const -> ARX::Buffer const &
{
	return m_Buffer;
}

auto ARX::BufferReader::GetPosition() const -> ARX::Buffer::size_type
{
	return m_Marker.GetPosition();
}

auto ARX::BufferReader::SetPosition(ARX::Buffer::size_type Position) -> void
{
	return m_Marker.SetPosition(Position);
}

auto ARX::operator>>(ARX::BufferReader & BufferReader, std::string & String) -> ARX::BufferReader &
{
	auto const & Buffer = BufferReader.GetBuffer();
	auto Index = BufferReader.GetPosition();
	auto Length = Buffer.GetLength();

	while(Length > Index)
	{
		if(auto Byte = Buffer[Index++]; Byte == std::byte{0})
		{
			break;
		}
        else
        {
            String += static_cast<std::string::value_type>(Byte);
        }
	}
	BufferReader.SetPosition(Index);

	return BufferReader;
}

auto ARX::operator>>(ARX::BufferReader & BufferReader, float & Value) -> ARX::BufferReader &
{
	BufferReader.Read(sizeof(float), reinterpret_cast<ARX::Buffer::pointer>(&Value));
	
	return BufferReader;
}

auto ARX::operator>>(ARX::BufferReader & BufferReader, std::uint8_t & Value) -> ARX::BufferReader &
{
	BufferReader.Read(sizeof(std::uint8_t), reinterpret_cast<ARX::Buffer::pointer>(&Value));
	
	return BufferReader;
}

auto ARX::operator>>(ARX::BufferReader & BufferReader, std::uint32_t & Value) -> ARX::BufferReader &
{
	BufferReader.Read(sizeof(std::uint32_t), reinterpret_cast<ARX::Buffer::pointer>(&Value));
	
	return BufferReader;
}

auto ARX::operator>>(ARX::BufferReader & BufferReader, std::uint64_t & Value) -> ARX::BufferReader &
{
	BufferReader.Read(sizeof(std::uint64_t), reinterpret_cast<ARX::Buffer::pointer>(&Value));
	
	return BufferReader;
}

auto ARX::operator>>(ARX::BufferReader & BufferReader, bool & Value) -> ARX::BufferReader &
{
	BufferReader.Read(sizeof(bool), reinterpret_cast<ARX::Buffer::pointer>(&Value));
	
	return BufferReader;
}
