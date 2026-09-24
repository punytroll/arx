/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__BUFFER_H
#define ARX__INCLUDE__BUFFER_H

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <arx/common.h>

/**
 * @file buffer.h
 * 
 * Declares the interface of ARX::Buffer.
 **/

namespace ARX
{
	/**
	 * @brief The core class of the Buffer.
	 *
	 * This class provides the concept of nested buffers.
     * That means: you create one main buffer and it will behave like any other ordinary buffer you know.
     * It is autoresizing as you add data, it provides prepend and append function, it has a position pointer which can be placed anywhere in the buffer to allow appending to that particular position.
	 *
	 * Additionally to that the nested buffers allow you to create compositions of buffers all acting on the same data block.
	 *
	 * Imagine a buffer of the size 16 bytes (name: Superior).
     * Although this data may be handled as one entity, you, as the user of the Buffers library, know the semantics of this buffer.
     * It may actually be three entities of data:
	 * - a 7-byte long block
	 * - a 5-byte long block
	 * - a 4-byte long block
	 * 
	 * Suppose now, that there is a library which (in any way) allows handling of a certain kind of data blocks.
     * In our example it may be the first block of length 7.
     * What you have to do is pass the buffer to the library, but this will violate the information hiding principle since this library now has access to the other datablock as well, maybe not even realizing that it isn't meant to meddle with them.
	 *
	 * Here comes the Buffers library which allows you to create sub buffers of our Superior buffer.
     * By creating a buffer via Buffer(Superior, 0, 7)  you get a sub buffer with length 7 and I/O position at 0 which you may pass to the library.
     * The content of this sub buffer is a reference to the superior buffer and thus contains exactly what is in the superior buffer from 0 to 7 at any given time.
     * Now the library is only able to access the content it is meant to see.
	 *
	 * But this concept strives for more functionality.
     * So it is also possible to perform any operations like appending, inserting and deleting.
     * Using these operations will affect the content of the superior buffer but the effect will be limited to the region of the sub buffer.
     * Any increases of the size of the sub buffer will also expand the superior buffer by the same amount.
	 * 
	 * The concept of buffers doesn't end at the first level of depth.
     * They allow you to nest sub buffers in sub buffers, thus creating a buffer structure of arbitrary depth and complexity.
	 * 
	 * For that reason all operations that work on the buffer have to be implemented recursively.
     * If you change a portion of data in a sub buffer, that change has to be propagated to the most superior buffer, which actually owns the associated memory.
     * Only this sup buffer is allowed to change the content of the memory and in turn it has to ensure that the effect of any such change will be passed down to the leaf childs.
	 **/
	class Buffer
	{
	public:
		friend std::string Indentation(ARX::Buffer * Buffer);
        
		/**
		 * @brief The type of an element stored in this buffer.
		 *
		 * Use Buffers::Buffer::value_type to work with the elements to be safe from API changes.
		 **/
		typedef std::byte value_type;
		
		/**
		 * @brief The type of a reference to an element stored in this buffer.
		 *
		 * Use Buffers::Buffer::reference to work with the elements to be safe from API changes.
		 **/
		typedef value_type & reference;
		
		/**
		 * @brief The type of a pointer to elements stored in this buffer.
		 *
		 * Use Buffers::Buffer::pointer to work with arrays of elements to be safe from API changes.
		 **/
		typedef value_type * pointer;
		
		/**
		 * @brief The type of a const element stored in this buffer.
		 *
		 * Use Buffers::Buffer::const_value_type to work with const elements in the buffer to be safe from API changes.
		 **/
		typedef const value_type const_value_type;
		
		/**
		 * @brief The type of a const pointer to elements stored in this buffer.
		 *
		 * Use Buffers::Buffer::const_pointer to work with const arrays of elements in the buffer to be safe from API changes.
		 **/
		typedef const_value_type * const_pointer;
		
		/**
		 * @brief The type of positions, lengths and sizes in this buffer.
		 *
		 * Use Buffers::Buffer::size_type when working with positions, sizes and length in a buffer to be safe from API changes.
		 **/
		typedef std::uint32_t size_type;
		
		/**
		 * @brief A marked position in a buffer, changing with content changes.
		 **/
		class Marker
		{
		public:
            enum class Alignment
			{
				Left,
				Right
			};
			
			/**
			 * @brief The Marker's constructor.
			 * 
			 * The constructor will call ARX::Buffer::Register(this) to register the marker at the associated ARX::Buffer.
			 * 
			 * The position is truncated at Buffer.GetLength() if greater than that.
			 **/
			explicit Marker(const ARX::Buffer & Buffer, ARX::Buffer::size_type Position = 0, ARX::Buffer::Marker::Alignment Alignment = ARX::Buffer::Marker::Alignment::Left);
			
			/**
			 * @brief The Marker's destructor.
			 * 
			 * The destructor will call ARX::Buffer::Unregister(this) to unregister the marker at the associated ARX::Buffer.
			 **/
			~Marker();
			
			/**
			 * @brief Returns the marker's current position inside the buffer.
			 * 
			 * The position is guaranteed to be valid inside the buffer.
			 **/
			auto GetPosition() const -> ARX::Buffer::size_type;
			
			/**
			 * @brief Sets the marker's position in the buffer.
			 * 
			 * Will not complain about the position being behind the buffer's end but will adjust to the length.
			 **/
			auto SetPosition(ARX::Buffer::size_type Position) -> void;
			
			/**
			 * @brief Returns the marker's alignment.
			 * 
			 * @sa ARX::Buffer::Marker::m_Alignment
			 **/
			auto GetAlignment() const -> ARX::Buffer::Marker::Alignment;
			
			/**
			 * @brief Sets the marker's alignment.
			 * 
			 * @sa ARX::Buffer::Marker::m_Alignment
			 **/
			auto SetAlignment(ARX::Buffer::Marker::Alignment Alignment) -> void;
			
			/**
			 * @brief Returns whether the marker is valid.
			 * 
			 * Currently the only occasion when a marker might be invalid is that the associated ARX::Buffer got destroyed.
			 **/
			auto IsValid() const -> bool;
            
		private:
			/**
			 * @brief Invalidates the marker's associated buffer.
			 * 
			 * This function sets m_Buffer to `nullptr`.
			 * 
			 * Calling this function invalidates the marker.
			 **/
			auto m_InvalidateBuffer() -> void;
            
			/**
			 * @brief A pointer to the associated ARX::Buffer.
			 * 
			 * Is initialized by the constructor and might be set to `nullptr` by ARX::Buffer::Marker::InvalidateBuffer().
			 **/
			ARX::Buffer const * m_Buffer;
			
			/**
			 * @brief The marker's current position inside the associated ARX::Buffer.
			 **/
			ARX::Buffer::size_type m_Position;
			
			/**
			 * @brief The marker's alignment.
			 * 
			 * The alignment of a marker can be @em Left or @em Right and indicates where the marker moves if data is inserted at the marker's position.
			 * - @em Left alignment means, that the marker will be at the beginning of the inserted data.
             *   The byte at the position of the marker will be the first byte of the inserted data.
             *   The old data at the position of the marker will be pushed to the back of the buffer by the new data.
			 * - @em Right alignments means, that the marker will be behind the end of the inserted data.
             *   The byte at the position of the marker will remain the same.
             *   The new data will be before the marker position.
			 * 
			 * @note The Alignment property is only relevant if data is inserted directly at the marker's position.
			 **/
			ARX::Buffer::Marker::Alignment m_Alignment;
                
		};
		
		friend class ARX::Buffer::Marker;
		
		/**
		 * @brief The standard constructor.
		 * 
		 * Constructs a buffer of size 0, I/O position is 0, overwrite state is false.
		 **/
		Buffer() = default;
		
		/**
		 * @brief The sub buffer constructor.
		 * @param Buffer The superior buffer.
		 * @param Position The sub buffer's begining position in the superior buffer.
		 * @param Length The length that should be covered inside the superior buffer starting from @a Position.
		 *
		 * Use this constructor if you want to have a sub buffer of the superior buffer @a Buffer.
         * It may be of length 0 to get an insertion spot at a later time.
		 **/
		Buffer(Buffer & Buffer, ARX::Buffer::size_type Position, ARX::Buffer::size_type Length);
        
		/**
		 * @brief Deleted copy constructor, to prevent copying a buffer.
		 **/
		Buffer(ARX::Buffer const & Other) = delete;
		
		/**
		 * @brief The destructor of a buffer. It is virtual since the Buffer may be overloaded to support external data.
		 * 
		 * The destructor of a buffer deletes it's content if it is the most superior buffer.
         * Otherwise it will unregister itself from its superior buffer.
		 *
		 * @note The destructor of a buffer will not touch its sub buffers (e.g. to delete them).
         * Currently it will not even notify them that their reference buffers are dead.
         * That should change.
		 **/
		virtual ~Buffer();
		
		/**
		 * @brief Deleted copy assignment operator, to prevent assignment of a buffer.
		 **/
		auto operator=(ARX::Buffer const & Buffer) -> ARX::Buffer & = delete;
		
		/**
		 * @brief Returns the length of the buffer.
		 * 
		 * This length is the length of the data inside the buffer, so the number of bytes actually used by the data.
		 **/
		auto GetLength() const -> ARX::Buffer::size_type;
		
		/**
		 * @brief Sets the length of the data actually in the buffer.
		 * @param Length The new length of the data.
		 *
		 * This is a convenience function as explained below:
		 * 
		 * - If the specified length is greater than the current length the buffer will be expanded to this length.
         *   The call is then equivalent to:
		 *   @code Insert(GetLength(), Length - GetLength()); @endcode
		 *
		 * - If the specified length is smaller than the current length the buffer will be shrunken to this length.
         *   This will happen back to front, so all child buffers which reside anywhere between @a Length and @a GetLength() will be modified as well.
         *   This call is then equivalent to:
         *   @code Delete(Length, GetLength() - Length); @endcode
		 **/
		auto SetLength(ARX::Buffer::size_type Length = 0) -> void;
		
		/**
		 * @brief Insert a given amount of data at a specified postion.
		 * @param Position The position inside the buffer.
		 * @param DataLength The length of the data block identified by @a Data.
		 * @param Data The block of data to be inserted in the buffer.
         *        This parameter may be omitted which will result in a null-pointer, indicating that the inserted block of data will be filled with Zeros.
		 *
		 * The data which is defined by @a Data will be inserted at position @a Position.
         * The data is taken to be of length @a DataLength at minimum.
         * The buffer will be expanded to contain @a GetLength() + @a DataLength bytes.
		 *
		 * @note This function directly calls the private function @a Write(), so this function throws the same exceptions as @a Write().
		 **/
		auto Insert(ARX::Buffer::size_type Position, ARX::Buffer::size_type DataLength, ARX::Buffer::const_pointer Data = nullptr) -> void;
		
		/**
		 * @brief Deletes a given amount of data at a specified position.
		 * @param Position The position inside the buffer.
		 * @param Length The length of the block that should be deleted.
		 * 
		 * This function deletes @a Length elements at position @a Position.
		 **/
		auto Delete(ARX::Buffer::size_type Position, ARX::Buffer::size_type Length) -> void;
		
		/**
		 * @brief Allows read-only access to indexed data members for const buffers.
		 * @param Index The index of the data member to access.
         *              Begining with zero.
		 *
		 * Returns a copy of the data member with index @a Index.
         * This function will throw a std::out_of_range error if @a Index is beyond @a GetLength().
		 **/
		auto operator[](ARX::Buffer::size_type Index) const -> ARX::Buffer::value_type;
		
		/**
		 * @brief Allows read/write access to indexed data members.
		 * @param Index The index of the data member to access.
         *              Begining with zero.
		 *
		 * Returns a refernece to the data member with index @a Index.
         * This function will throw a std::out_of_range error if @a Index is beyond @a GetLength().
		 **/
		auto operator[](ARX::Buffer::size_type Index) -> ARX::Buffer::reference;
		
		/**
		 * @brief Retreives a const pointer to the data of this buffer.
		 **/
		auto GetBegin() const -> ARX::Buffer::const_pointer;
        
	private:
		class SubBuffer;
		
		/**
		 * @brief This function registers a buffer to be a sub buffer of @em this buffer.
		 *
		 * This function handles the necessary steps to organise the control structure which contains all the sub buffers.
         * This structure is needed to propagate changes.
		 **/
		auto m_Register(ARX::Buffer & SubBuffer) -> void;
		
		/**
		 * @brief This function removes a buffer from the list of all sub buffer of @em this buffer.
		 *
		 * This function handles the necessary steps to organise the control structure which contains all the sub buffers.
         * This structure is needed to propagate changes.
		 **/
		auto m_Unregister(ARX::Buffer & SubBuffer) -> void;
		
		auto m_Register(ARX::Buffer::Marker & Marker) const -> void;
		
		auto m_Unregister(ARX::Buffer::Marker & Marker) const -> void;
		
		/**
		 * @brief This function is responsible for any functionality that inserts data to the buffer.
		 * @param Buffer The original buffer that made the insertion request.
		 * @param Position The position where the data is to be inserted, modified to this buffer.
		 * @param DataLength The length of the data block @a Data.
		 * @param Data The data block to be written into the buffer.
		 * 
		 * This function is one of the two core functions of the buffer. It allows to insert any data in any buffer and will pipe the call to the most superior buffer which actually copies the data. It will then decide which sub buffers should be notified that content or position has changed.
		 **/
		auto m_Insert(ARX::Buffer & Buffer, ARX::Buffer::size_type Position, ARX::Buffer::size_type DataLength, ARX::Buffer::const_pointer Data = nullptr) -> void;
		
		/**
		 * @brief This function is called whenever data has been deleted in the sup buffer, in a way that also affects @em this buffer.
		 * @param Position Indicates the location of the deletion inside the sup buffer.
		 * @param Length Specifies the length of the deletion.
		 * 
         * Because of its scope, @a Position is comparable with @a m_Position.
		 * 
		 * - The local changes are defined by the position and length of the deletion. There are six possibilities:
		 *   - The deletion is completely before @em this buffer: only @a m_Position needs to be decremented by stLength.
		 *   - The deletion begins before @em this buffer and overlaps the beginning: @a m_Position needs to be set to @a Position, @a m_Length needs to be reduced by the overlapping.
		 *   - The deletion is completely inside @em this buffer: @a m_Length needs to be reduced by @a Length.
		 *   - The deletion begins inside @em this buffer and overlaps the end: @a m_Length needs to be reduced by the size of the overlapping.
		 *   - The deletion is completely behind @em this buffer: nothing to be done.
		 *   - The deletion overlaps @em this buffer completely: @a m_Length is set to 0 and @a m_Position is set to @a Position.
		 * - To change any sub buffers, we just need to pass this change down to all sub buffers by calling m_ParentDataDeleted(Position - m_Position, Length).
		 **/
        auto m_ParentDataDeleted(ARX::Buffer::size_type Position, ARX::Buffer::size_type Length) -> void;
        
		/**
		 * @brief This function is called whenever data has been inserted in the sup buffer, in a way that also affects @em this buffer.
		 * @param Position Indicates the location of the insertion inside the sup buffer.
		 * @param Length Specifies the length of the insertion.
		 * @note Notice that this function is called even if the insert was not inside @em this buffer.
         *       If the insertion was @em before this buffer (@a Position < @a m_Position) we need to reset @a m_Position.
		 * 
		 * - The local changes depend on the position of the insertion.
         *   If the data was inserted before this buffer we need to reset m_Position and m_Begin.
         *   If the data was inserted inside the buffer we need to reset m_Length.
		 * - To change any sub buffers, we just pass the change by calling m_ParentDataInserted(Position - m_Position, Length).
         * There is one exception to this:
         * If @em this buffer made the initial change and the change request was passed up to the most superior buffer and now the changes are passed down again we don't apply changes here.
         * Once all the changes are done and we return to the initial insertion function it is done then.
		 **/
        auto m_ParentDataInserted(ARX::Buffer::size_type Position, ARX::Buffer::size_type Length) -> void;
        
		/**
		 * @brief This function is called whenever the data of the sup buffer has been updated.
         * 
         * Resets the m_Begin pointer to the correct location inside the sup buffer (m_SupBuffer->m_Begin + m_Position).
		 * Notifies all sub buffers recursively.
         **/
        auto m_ParentDataUpdated() -> void;
        
		/**
		 * @brief A pointer pointing to the superior buffer.
		 *
		 * This pointer points to the Buffer that this buffer is a part of. In case this buffer is the most superior buffer it is 'nullptr'.
		 **/
		ARX::Buffer * m_SupBuffer{nullptr};
		
		/**
		 * @brief The pointer to the actual content of @em this buffer.
		 **/
		ARX::Buffer::pointer m_Begin{nullptr};
		
		/**
		 * @brief The length of the content within @em this buffer.
		 *
		 * @note There is a separate property m_Capacity for the size of the allocated memory.
		 **/
		ARX::Buffer::size_type m_Length{0};
		
		/**
		 * @brief The size of the memory block reserved for the content of @em this buffer.
		 * 
		 * This is an internal-only data. It describes how much data may be stored in @em this buffer currently without having to reallocate the buffers' memory.
		 *
		 * @note There is a separate property m_Length for the length of the content.
		 **/
		ARX::Buffer::size_type m_Capacity{0};
		
		/**
		 * @brief The associated markers.
		 **/
		mutable std::set<ARX::Buffer::Marker *> m_Markers;
		
		/**
		 * @brief The position of the begin of @em this buffer itside its superior buffer.
		 *
		 * This value is zero-based.
		 **/
		ARX::Buffer::size_type m_Position{0};
        
		/**
		 * @brief A vector of pointers to structures describing the sub buffers.
		 *
		 * The order of these pointers is irrelevant except for one thing: the last sub buffer must always have the greatest m_stOrder value because it will be the reference for the m_stOrder of new registering buffers.
		 **/
		std::vector<ARX::Buffer::SubBuffer *> m_SubBuffers;
		
		/**
		 * @brief Indicates the changing state of @em this buffer.
		 *
		 * This is a fluid state and will be changed in vWrite() and vDelete() when @em this buffer is actually changing. It will not be true from anywhere outside the library.
		 **/
		bool m_Changing{false};
        
	};
	
	/**
	 * @brief An output operator for std::ostream which uses the Buffers::Buffer::Ouput function.
	 * 
	 * This operator is implemented for your convinience and allows you to write:
	 * @code
	 * std::cout << Buffer;
	 * @endcode
	 * or any other std::ostream object. Remember that the data inside the buffer may be binary and thus is not meant to be pushed to std::cout.
	 **/
	auto operator<<(std::ostream & OStream, ARX::Buffer const & Buffer) -> std::ostream &;
};

#endif
