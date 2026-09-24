/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__BUFFERREADER_H
#define ARX__INCLUDE__BUFFERREADER_H

#include <cstdint>

#include <arx/buffer.h>

/**
 * @file buffer_reader.h
 * 
 * This file declares the interface for the ARX::BufferReader class.
 **/

namespace ARX
{
	/**
	 * @brief Implements a convenient output interface for ARX::Buffer.
	 **/
	class BufferReader
	{
	public:
		/**
		 * @brief The BufferReader constructor.
		 * @note Sets the read position to the beginning of the buffer.
		 **/
		explicit BufferReader(ARX::Buffer const & Buffer);
        
		/**
		 * @brief The BufferReader constructor.
		 * @note Sets the read position to the indicated position.
		 **/
		BufferReader(ARX::Buffer const & Buffer, ARX::Buffer::size_type Position);
		
		/**
		 * @brief A copy constructor for the BufferReader.
		 * 
		 * This constructor creates a second BufferReader which refers to the same Buffer but using its own marker at the same location.
         * The new BufferReader behaves as if created on its own.
		 **/
		BufferReader(ARX::BufferReader const & Other);
		
		/**
		 * @brief This functions allows to get more than one byte at a time from the buffer.
		 * @param Length The length of the chunk of data you wish to read from the buffer.
		 * @param Buffer Not a Buffer but a buffer, an address of a block of memory.
		 *
		 * This function reads @am Length bytes from @em this Buffer starting at the I/O position and writes them to the memory specified by @am Buffer.
		 * This function does not give you access to the content of the buffer but creates of copy of it.
		 *
		 * @note This function refuses to read more elements than there are from the I/O position to the end.
         *       It will throw a std::out_of_range exception in this case.
		 **/
		auto Read(ARX::Buffer::size_type Length, ARX::Buffer::pointer Buffer) -> ARX::Buffer::size_type;
		
		/**
		 * @brief Returns the associated ARX::Buffer object.
		 **/
		auto GetBuffer() const -> ARX::Buffer const &;
		
		/**
		 * @brief Returns the marker's position.
		 **/
		auto GetPosition() const -> ARX::Buffer::size_type;
		
		/**
		 * @brief Sets the marker's position.
		 **/
		auto SetPosition(ARX::Buffer::size_type Position) -> void;
        
	private:
		ARX::Buffer const & m_Buffer;
		ARX::Buffer::Marker m_Marker;
        
	};
	
	/**
	 * @brief A helper function for reading a string from a buffer.
	 * @param BufferReader The BufferReader from which the string will be read.
	 * @param String A std::string which will be filled with the string from the buffer.
	 *
	 * This function reads a string from the @a BufferReader.
     * The string may be either null-terminated or end at the end of the buffer.
	 *
	 * @note This function will read until it finds either the first null character in the buffer or the end of the buffer so that the string itself cannot contain a null character.
	 **/
	auto operator>>(ARX::BufferReader & BufferReader, std::string & String) -> ARX::BufferReader &;
	
	/**
	 * @brief A helper function for reading float values from a buffer.
	 * @param BufferReader The BufferReader from which the float value will be read.
	 * @param Value The target for the float value that is to be read.
	 * @return The Buffer after the output execution.
	 * 
	 * This function reads a float value from the buffer storing it in @a Value .
	 **/
	auto operator>>(ARX::BufferReader & BufferReader, float & Value) -> ARX::BufferReader &;
	
	/**
	 * @brief A helper function for reading 1 byte unsigned integer values from a buffer.
	 * @param BufferReader The BufferReader from which the 1 byte unsigned integer value will be read.
	 * @param Value The target for the 1 byte unsigned integer value that is to be read.
	 * @return The Buffer after the output execution.
	 * 
	 * This function reads a 1 byte unsigned integer value from the buffer storing it in @a Value .
	 **/
	auto operator>>(ARX::BufferReader & BufferReader, std::uint8_t & Value) -> ARX::BufferReader &;
	
	/**
	 * @brief A helper function for reading 4 byte unsigned integer values from a buffer.
	 * @param BufferReader The BufferReader from which the 4 byte unsigned integer value will be read.
	 * @param Value The target for the 4 byte unsigned integer value that is to be read.
	 * @return The Buffer after the output execution.
	 * 
	 * This function reads a 4 byte unsigned integer value from the buffer storing it in @a Value .
	 **/
	auto operator>>(ARX::BufferReader & BufferReader, std::uint32_t & Value) -> ARX::BufferReader &;
	
	/**
	 * @brief A helper function for reading 8 byte unsigned integer values from a buffer.
	 * @param BufferReader The BufferReader from which the 8 byte unsigned integer value will be read.
	 * @param Value The target for the 8 byte unsigned integer value that is to be read.
	 * @return The Buffer after the output execution.
	 * 
	 * This function reads an 8 byte unsigned integer value from the buffer, storing it in @a Value .
	 **/
	auto operator>>(ARX::BufferReader & BufferReader, std::uint64_t & Value) -> ARX::BufferReader &;
	
	/**
	 * @brief A helper function for reading bool objects from a buffer.
	 * @param BufferReader The BufferReader from which the bool will be read.
	 * @param Value The target for the bool value that is to be read.
	 * @return The Buffer after the output execution.
	 * 
	 * This function reads the bool value @a Value from the buffer using an output function.
	 **/
	auto operator>>(ARX::BufferReader & BufferReader, bool & Value) -> ARX::BufferReader &;
}

#endif
