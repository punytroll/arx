/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__BUFFERREADER_H
#define ARX__INCLUDE__BUFFERREADER_H

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
         * @brief Returns the associated ARX::Buffer object.
         **/
        auto GetBuffer() const -> ARX::Buffer const &;
        
        /**
         * @brief Returns the marker's position.
         **/
        auto GetPosition() const -> ARX::Buffer::size_type;
        
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
         * @brief Sets the marker's position.
         **/
        auto SetPosition(ARX::Buffer::size_type Position) -> void;
        
        /**
         * @brief Advances the marker's position by the given amount of bytes.
         **/
        auto Skip(ARX::Buffer::size_type Offset) -> void;
        
    private:
        ARX::Buffer const & m_Buffer;
        ARX::Buffer::Marker m_Marker;
    };
}

#endif
