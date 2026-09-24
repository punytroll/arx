/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <string.h>

#include <istream>

#include <arx/buffer.h>
#include <arx/buffer_writer.h>

#ifdef DEBUG
#include <iostream>
#endif

ARX::BufferWriter::BufferWriter(ARX::Buffer & Buffer) :
	m_Buffer{Buffer},
	m_Marker{Buffer, Buffer.GetLength(), ARX::Buffer::Marker::Alignment::Left}
{
}

ARX::BufferWriter::BufferWriter(ARX::Buffer & Buffer, ARX::Buffer::size_type Position) :
	m_Buffer{Buffer},
	m_Marker{Buffer, Position, ARX::Buffer::Marker::Alignment::Left}
{
}

ARX::BufferWriter::BufferWriter(ARX::BufferWriter const & Other) :
	ARX::BufferWriter::BufferWriter{Other.m_Buffer, Other.GetPosition()}
{
}

auto ARX::BufferWriter::Write(ARX::Buffer::size_type DataLength, ARX::Buffer::const_pointer Data) -> void
{
	if(m_Marker.IsValid() == false)
	{
		throw std::runtime_error{"ARX::BufferWriter::vWrite: Buffer was destroyed."};
	}
	m_Marker.SetAlignment(ARX::Buffer::Marker::Alignment::Right);
	m_Buffer.Insert(m_Marker.GetPosition(), DataLength, Data);
	m_Marker.SetAlignment(ARX::Buffer::Marker::Alignment::Left);
}

auto ARX::BufferWriter::GetBuffer() -> ARX::Buffer &
{
	return m_Buffer;
}

auto ARX::BufferWriter::GetBuffer() const -> ARX::Buffer const &
{
	return m_Buffer;
}

auto ARX::BufferWriter::GetPosition() const -> ARX::Buffer::size_type
{
	return m_Marker.GetPosition();
}

auto ARX::BufferWriter::SetPosition(ARX::Buffer::size_type Position) -> void
{
	return m_Marker.SetPosition(Position);
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, std::string const & String) -> ARX::BufferWriter &
{
	BufferWriter.Write(String.length() + 1, reinterpret_cast<ARX::Buffer::const_pointer>(String.c_str()));

	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, char const * String) -> ARX::BufferWriter &
{
	BufferWriter.Write(strlen(String) + 1, reinterpret_cast<ARX::Buffer::const_pointer>(String));

	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, float Value) -> ARX::BufferWriter &
{
	BufferWriter.Write(sizeof(float), reinterpret_cast<ARX::Buffer::const_pointer>(&Value));
	
	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, std::uint8_t Value) -> ARX::BufferWriter &
{
	BufferWriter.Write(sizeof(std::uint8_t), reinterpret_cast<ARX::Buffer::const_pointer>(&Value));
	
	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, std::uint32_t Value) -> ARX::BufferWriter &
{
	BufferWriter.Write(sizeof(std::uint32_t), reinterpret_cast<ARX::Buffer::const_pointer>(&Value));
	
	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, std::uint64_t Value) -> ARX::BufferWriter &
{
	BufferWriter.Write(sizeof(std::uint64_t), reinterpret_cast<ARX::Buffer::const_pointer>(&Value));
	
	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, bool Value) -> ARX::BufferWriter &
{
	BufferWriter.Write(sizeof(bool), reinterpret_cast<ARX::Buffer::const_pointer>(&Value));
	
	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, char Value) -> ARX::BufferWriter &
{
	BufferWriter.Write(sizeof(char), reinterpret_cast<ARX::Buffer::const_pointer>(&Value));
	
	return BufferWriter;
}

auto ARX::operator<<(ARX::BufferWriter & BufferWriter, std::pair<ARX::Buffer::size_type, std::istream *> const & Stream) -> ARX::BufferWriter &
{
#ifdef DEBUG
	std::cerr << "ARX::operator<<(std::pair< size, stream >): size = " << Stream.first << std::endl;
#endif
	auto Index = 0U;

	while(Index < Stream.first)
	{
		auto Character = '\0';
		
		Stream.second->get(Character);
		if(Stream.second->good() == true)
		{
			++Index;
			BufferWriter.Write(1, reinterpret_cast<ARX::Buffer::const_pointer>(&Character));
		}
		else
		{
			break;
		}
	}
#ifdef DEBUG
	std::cerr << "ARX::operator<<(std::pair< size, stream >): read = " << Index << std::endl;
#endif

	return BufferWriter;
}
