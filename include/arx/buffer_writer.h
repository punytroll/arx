/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__BUFFERWRITER_H
#define ARX__INCLUDE__BUFFERWRITER_H

#include <cstdint>

#include <arx/buffer.h>

/**
 * @file buffer_writer.h
 * 
 * This file declares the interface for the ARX::BufferWriter class.
 **/

namespace ARX
{
	/**
	 * @brief Implements a convenient input interface for ARX::Buffer.
	 * 
	 * The BufferWriter is implemented using a ARX::Buffer::Marker to indicate the position at which the data is to be inserted into the buffer.
	 * 
	 * @note This also means that a BufferWriter is not referring to an index into the buffer but to a position in the data inside the buffer.
     *       Whenever changes occure to the data the input position will change in the same way that a ARX::Buffer::Marker would.
     *       The default alignment of the Marker representing the insert position is Left, so that data changes at the insert position of the BufferWriter will not affect the marker.
     *       However if you insert data using the BufferWriter, the Alignment of the marker is set to Right for the time of the insert thus moving the insert position behind the inserted data.
	 **/
	class BufferWriter
	{
	public:
        
		/**
		 * @brief The BufferWriter constructor.
		 * @note Sets the write position to the end of the buffer.
		 **/
		explicit BufferWriter(ARX::Buffer & Buffer);
        
		/**
		 * @brief The BufferWriter constructor.
		 * @note Sets the write position to the indicated position.
		 **/
		BufferWriter(ARX::Buffer & Buffer, ARX::Buffer::size_type Position);
		
		/**
		 * @brief A copy constructor for the BufferWriter.
		 * 
		 * This constructor creates a second BufferWriter which refers to the same Buffer but using an own marker.
         * The new BufferWriter behaves as if created on its own.
		 **/
		BufferWriter(ARX::BufferWriter const & Other);
		
		/**
		 * @brief Write a block of data at the writer's position in the buffer.
		 * @param DataLength The length of the data block @a Data
		 * @param Data The data block to insert at the writer's position.
         *             May be omitted to insert @a DataLength zeroed bytes.
		 **/
		auto Write(ARX::Buffer::size_type DataLength, ARX::Buffer::const_pointer Data = nullptr) -> void;
		
		/**
		 * @brief Returns the associated ARX::Buffer object.
		 * @return A non-const ARX::Buffer reference.
		 **/
		auto GetBuffer() -> ARX::Buffer &;
		
		/**
		 * @brief Returns the associated ARX::Buffer object.
		 * @return A const ARX::Buffer reference.
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
		ARX::Buffer & m_Buffer;
		ARX::Buffer::Marker m_Marker;
        
	};

	/**
	 * @brief A helper function for storing a std::string instance in a buffer.
	 * @param BufferWriter The buffer in which the string will be stored.
	 * @param String The string which is about to be stored.
	 * @return The BufferWriter after the input execution.
	 *
	 * This helper function stores the std::string @a String in the buffer @a Buffer.
     * It uses the Buffers::Buffer::Input function so that the buffer current I/O position will be used as the insertion position.
     * Therefore also the overwrite state of the buffer will be adhered.
     * Afterwards the I/O position will be set to the end of the insertion by Buffer::Buffer::Input().
	 *
	 * @note The string will be stored as a zero-terminated string so that a trailing zero will be appended.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, std::string const & String) -> ARX::BufferWriter &;

	/**
	 * @brief A helper function for storing a C string in a buffer.
	 * @param BufferWriter The buffer in which the C string will be stored.
	 * @param String The C string which is about to be stored.
	 * @return The BufferWriter after the input execution.
	 *
	 * This helper function stores the C string @a String in the buffer @a Buffer.
	 * The function uses strlen(3) to determine the length of the string.
	 *
	 * @note The string will be stored as a zero-terminated string so that a trailing zero will be appended.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, const char * String) -> ARX::BufferWriter &;
	
	/**
	 * @brief A helper function for storing float numbers in a buffer.
	 * @param BufferWriter The buffer in which the float number will be stored.
	 * @param Value The float value which is to be stored.
	 * @return The BufferWriter after the input execution.
	 * 
	 * This function stores the float value @a Value in the buffer using an input function.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, float Value) -> ARX::BufferWriter &;
	
	/**
	 * @brief A helper function for storing 1 byte unsigned integer values in a buffer.
	 * @param BufferWriter The buffer in which the 1 byte unsigned integer will be stored.
	 * @param Value The 1 byte unsigned integer value which is to be stored.
	 * @return The BufferWriter after the input execution.
	 * 
	 * This function stores the 1 byte unsigned integer value @a Value in the buffer using an input function.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, std::uint8_t Value) -> ARX::BufferWriter &;
	
	/**
	 * @brief A helper function for storing 4 byte unsigned integer values in a buffer.
	 * @param BufferWriter The buffer in which the 4 byte unsigned integer will be stored.
	 * @param Value The 4 byte unsigned integer value which is to be stored.
	 * @return The BufferWriter after the input execution.
	 * 
	 * This function stores the 4 byte unsigned integer value @a Value in the buffer using an input function.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, std::uint32_t Value) -> ARX::BufferWriter &;
	
	/**
	 * @brief A helper function for storing 8 byte unsigned integer values in a buffer.
	 * @param BufferWriter The buffer in which the 8 byte unsigned integer will be stored.
	 * @param Value The 8 byte unsigned integer value which is to be stored.
	 * @return The BufferWriter after the input execution.
	 * 
	 * This function stores the 8 byte unsigned integer value @a Value in the buffer using an input function.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, std::uint64_t Value) -> ARX::BufferWriter &;
	
	/**
	 * @brief A helper function for storing bool objects in a buffer.
	 * @param BufferWriter The buffer in which the bool will be stored.
	 * @param Value The bool value which is to be stored.
	 * @return The BufferWriter after the input execution.
	 * 
	 * This function stores the bool value @a Value in the buffer using an input function.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, bool Value) -> ARX::BufferWriter &;
	
	/**
	 * @brief A helper function for storing char objects in a buffer.
	 * @param BufferWriter The buffer in which the bool will be stored.
	 * @param Value The char value which is to be stored.
	 * @return The BufferWriter after the input execution.
	 * 
	 * This function stores the char value @a Value in the buffer using an input function.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, char Value) -> ARX::BufferWriter &;
	
	/**
	 * @brief A helper function that allows filling a buffer from an std::istream.
	 * @param BufferWriter The buffer which will be filled from the content of the istream.
	 * @param Stream A std::pair which's first component describes the amount of bytes to be read from the stream specified in its second component.
	 * @return The BufferWriter after the input execution.
	 * 
	 * This function will append the stream content at the I/O position of the buffer.
     * It will read @a Stream.first bytes from the stream.
     * In case the stream end before that amount of bytes the buffer will contain only as much bytes as could be read from the stream.
     * This function is an I/O operation and thus will modify the I/O position of the buffer to point after the last byte read.
	 **/
	auto operator<<(ARX::BufferWriter & BufferWriter, std::pair<ARX::Buffer::size_type, std::istream *> const & Stream) -> ARX::BufferWriter &;
}

#endif
