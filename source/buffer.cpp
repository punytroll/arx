/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <iostream>

#include <arx/buffer.h>

#ifdef DEBUG
namespace ARX
{
	std::string Indentation(ARX::Buffer * Buffer)
	{
		std::string Pad = "";

		while(Buffer->m_SupBuffer)
		{
			Pad += "  ";
			Buffer = Buffer->m_SupBuffer;
		}

		return Pad;
	}
}
#endif

/**
 * @brief This class represents a sub buffer and its maintainance information inside a superior buffer.
 *
 * Whenever a sub buffer is created the superior buffer will add this informative structure to its m_SubBuffers vector.
 * Currently there is only one piece of information given here: the order.
 * The order of a sub buffer is a numeric value related to the time when it was created as a sub buffer.
 * It actually is not a time value.
 * It only supports the one relation:
 * - Buffer1.GetOrder() > Buffer2.GetOrder()  <=>  Buffer1 was created before Buffer2.
 * 
 * Why would anybody need such a thing?
 *
 * Imagine an arbitrary buffer, filled or empty, that doesn't matter.
 * Now mentally create three sub buffers of length 0 at the same position, one after another so that they get different orders.
 * Now consider the three (different) scenarios of inserting an amount of data into one of those buffers.
 * - When inserting into the first buffer (the one, you created first which got the lowest order) it would be nice to have the two remaining buffers at the end of the first buffer, not overlapping.
 * - When inserting into the second buffer it would be nice if the first buffer would still be in front of the second one and the third would be at its end.
 * - When inserting into the third buffer it would be nice if the two remaining buffer would be in front of the third.
 * 
 * Now, since all three buffer share the same values concerning their physical structure (position and length) there is no way of differentiating these three scenarios.
 * What we need is the only difference they have: their times of creation.
 **/
class ARX::Buffer::SubBuffer
{
public:
	/**
	 * @brief The constructor of a SubBuffer structure.
	 * @param Buffer The associated buffer which actually is the sub buffer.
	 * @param Order The order this sub buffer was inserted in the superior buffer.
	 * 
	 * This constructor creates a SubBuffer.
     * The parameters are the information the superior buffer whishes to associate with the sub buffer.
	 **/
	SubBuffer(ARX::Buffer & Buffer, size_type Order) :
		m_Buffer{Buffer},
		m_Order{Order}
	{
	}

	/**
	 * @brief A function to retrieve the associated buffer.
	 *
	 * With this function you can reget the associated sub buffer.
	 **/
	Buffer & GetBuffer()
	{
		return m_Buffer;
	}

	/**
	 * @brief A function to retrieve the associated order of the sub buffer.
	 *
	 * With this function you can get the associated order.
	 **/
	size_type GetOrder()
	{
		return m_Order;
	}
private:
	/**
	 * @brief The associated Buffer.
	 *
	 * A reference to the associated Buffer.
     * As it is a reference it cannot be re-set or left unspecified.
     * This class is only valid if there actually is such a sub buffer.
     * It must be set with the constructor and may be retrieved with @a GetBuffer().
	 **/
	Buffer & m_Buffer;

	/**
	 * @brief The associated order of the sub buffer.
	 *
	 * A numeric value describing the order of the associated buffer (@a m_Buffer) inside the superior buffer.
     * It may be retrieved via @a GetOrder().
	 **/
	size_type m_Order;
};

ARX::Buffer::Buffer(ARX::Buffer & Buffer, size_type Position, size_type Length) :
	m_SupBuffer{&Buffer},
	m_Position{(Buffer.GetLength() < Position) ? (Buffer.GetLength()) : (Position)}
{
	if(Buffer.m_Begin != nullptr)
	{
		m_Begin = Buffer.m_Begin + m_Position;
	}
	if(Buffer.GetLength() >= m_Position + Length)
	{
		m_Length = m_Capacity = Length;
	}
	else
	{
		m_Length = m_Capacity = Buffer.m_Length - m_Position;
	}
	Buffer.m_Register(*this);
}

ARX::Buffer::~Buffer()
{
	if(m_SupBuffer == nullptr)
	{
		delete[] m_Begin;
	}
	else
	{
		m_SupBuffer->m_Unregister(*this);
	}

	auto SubBufferIterator = m_SubBuffers.begin();

	while(SubBufferIterator != m_SubBuffers.end())
	{
		delete *SubBufferIterator;
		SubBufferIterator = m_SubBuffers.erase(SubBufferIterator);
	}
}

void ARX::Buffer::m_Register(ARX::Buffer & Buffer)
{
	auto Order = 1U;

	if(m_SubBuffers.size() != 0)
	{
		Order = (*(m_SubBuffers.end() - 1))->GetOrder() + 1;
	}
	m_SubBuffers.push_back(new ARX::Buffer::SubBuffer{Buffer, Order});
}

void ARX::Buffer::m_Unregister(ARX::Buffer & Buffer)
{
	auto SubBufferIterator = m_SubBuffers.begin();
	auto SubBufferEnd = m_SubBuffers.end();

	while((SubBufferIterator != SubBufferEnd) && (std::addressof((*SubBufferIterator)->GetBuffer()) != std::addressof(Buffer)))
	{
		++SubBufferIterator;
	}
	if(SubBufferIterator != SubBufferEnd)
	{
		delete *SubBufferIterator;
		m_SubBuffers.erase(SubBufferIterator);
	}
}

void ARX::Buffer::m_Register(ARX::Buffer::Marker & Marker) const
{
	m_Markers.insert(&Marker);
}

void ARX::Buffer::m_Unregister(ARX::Buffer::Marker & Marker) const
{
	m_Markers.erase(m_Markers.find(&Marker));
}

ARX::Buffer::size_type ARX::Buffer::GetLength(void) const
{
	return m_Length;
}

void ARX::Buffer::SetLength(size_type Length)
{
	if(Length != m_Length)
	{
		if(Length > m_Length)
		{
			Insert(m_Length, Length - m_Length);
		}
		else
		{
			Delete(Length, m_Length - Length);
		}
	}
}

void ARX::Buffer::Insert(size_type Position, size_type DataLength, const_pointer Data)
{
	m_Insert(*this, Position, DataLength, Data);
}

void ARX::Buffer::m_Insert(ARX::Buffer & Buffer, ARX::Buffer::size_type Position, size_type DataLength, const_pointer Data)
{
#ifdef DEBUG
	std::cerr << this << Indentation(this) << " Insert(Buffer = " << &Buffer << ", Position = " << Position << ", DataLength = " << DataLength << ")   ---   Status: Position = " << m_Position << ", Length = " << m_Length << ", Capacity = " << m_Capacity << ", ParentBuffer = " << m_SupBuffer << std::endl;
#endif
	if(Position > m_Length)
	{
		throw std::out_of_range{"Trying to write after the end."};
	}

	auto OldBegin = m_Begin;

	// if the following condition is met we are in the most superior buffer
	if(m_SupBuffer == nullptr)
	{
		// now the real work begins, we have to make the change.
		auto InsertedData = false;
		
		if(m_Length + DataLength > m_Capacity)
		{
			InsertedData = true;
            
			// more data than free space
			auto Length = std::max(m_Length * 2, m_Length + DataLength);
			auto Begin = new value_type[Length];
			auto End = std::copy(m_Begin, m_Begin + Position, Begin);
			
			if(Data != nullptr)
			{
				End = std::copy(Data, Data + DataLength, End);
			}
			else
			{
				End += DataLength;
			}
			End = std::copy(m_Begin + Position, m_Begin + m_Length, End);
			delete[] m_Begin;
			m_Begin = Begin;
			m_Capacity = Length;
		}
		else
		{
			InsertedData = true;
			std::copy_backward(m_Begin + Position, m_Begin + m_Length, m_Begin + m_Length + DataLength);
			if(Data != nullptr)
			{
				std::copy(Data, Data + DataLength, m_Begin + Position);
			}
		}
		m_Length += DataLength;
		for(auto Marker : m_Markers)
		{
			if(((Marker->GetPosition() == Position) && (Marker->GetAlignment() == ARX::Buffer::Marker::Alignment::Right)) || (Marker->GetPosition() > Position))
			{
				Marker->SetPosition(Marker->GetPosition() + DataLength);
			}
		}
		
		auto Order = 0U;

		if(std::addressof(Buffer) != this)
		{
			auto SubBufferIterator = m_SubBuffers.begin();
			auto SubBufferEnd = m_SubBuffers.end();
            
			while((SubBufferIterator != SubBufferEnd) && (std::addressof((*SubBufferIterator)->GetBuffer()) != std::addressof(Buffer)))
			{
				++SubBufferIterator;
			}
			if(SubBufferIterator == SubBufferEnd)
			{
				throw std::invalid_argument{"The Buffer seems to be invalid (it is not registered as a child)"};
			}
			Order = (*SubBufferIterator)->GetOrder();
		}
#ifdef DEBUG
		std::cerr << this << Indentation(this) << " is inserting with order " << Order << " and InsertedData = " << InsertedData << std::endl;
#endif
		for(auto SubBuffer : m_SubBuffers)
		{
			if((SubBuffer->GetOrder() >= Order) && (InsertedData == true))
			{
				SubBuffer->GetBuffer().m_ParentDataInserted(Position, DataLength);
			}
			else
			{
				if(m_Begin != OldBegin)
				{
					SubBuffer->GetBuffer().m_ParentDataUpdated();
				}
			}
		}
	}
	else
	{
		// this is fairly easy as we are in a child buffer
		//   we just have to pass the change further up.
		m_Changing = true;
		m_SupBuffer->m_Insert(*this, m_Position + Position, DataLength, Data);
		m_Changing = false;

		auto Order = 0U;

		if(&Buffer != this)
		{
			auto SubBufferIterator = m_SubBuffers.begin();
			auto SubBufferEnd = m_SubBuffers.end();
            
			while((SubBufferIterator != SubBufferEnd) && (std::addressof((*SubBufferIterator)->GetBuffer()) != std::addressof(Buffer)))
			{
				++SubBufferIterator;
			}
			if(SubBufferIterator == SubBufferEnd)
			{
				throw std::invalid_argument{"The Buffer seems to be invalid (it is not registered as a child)"};
			}
			Order = (*SubBufferIterator)->GetOrder();
		}
#ifdef DEBUG
		std::cerr << this << Indentation(this) << " is inserting with order " << Order << std::endl;
#endif
		for(auto SubBuffer : m_SubBuffers)
		{
			if(SubBuffer->GetOrder() >= Order)
			{
				SubBuffer->GetBuffer().m_ParentDataInserted(Position, DataLength);
			}
			else
			{
				if(m_Begin != OldBegin)
				{
					SubBuffer->GetBuffer().m_ParentDataUpdated();
				}
			}
		}
	}
}

void ARX::Buffer::Delete(size_type Position, size_type Length)
{
#ifdef DEBUG
	std::cerr << this << Indentation(this) << " Delete(Position = " << Position << ", Length = " << Length << ")   ---   Status: Position = " << m_Position << ", Length = " << m_Length << std::endl;
#endif
	if((Position > m_Length) || (Position + Length > m_Length))
	{
		throw std::out_of_range{"Trying to delete after the end or until after the end."};
	}
	if(m_SupBuffer == nullptr)
	{
		std::copy(m_Begin + Position + Length, m_Begin + m_Length, m_Begin + Position);
		m_Length -= Length;
		for(auto Marker : m_Markers)
		{
			if(Marker->GetPosition() > m_Length)
			{
				Marker->SetPosition(m_Length);
			}
		}
		for(auto SubBuffer : m_SubBuffers)
		{
			SubBuffer->GetBuffer().m_ParentDataDeleted(Position, Length);
		}
	}
	else
	{
		m_Changing = true;
		m_SupBuffer->Delete(m_Position + Position, Length);
		m_Changing = false;
	}
}

ARX::Buffer::const_pointer ARX::Buffer::GetBegin() const
{
	return m_Begin;
}
        
auto ARX::Buffer::m_ParentDataDeleted(ARX::Buffer::size_type Position, ARX::Buffer::size_type Length) -> void
{
#ifdef DEBUG
	std::cerr << this << Indentation(this) << " ParentDataDeleted(Position = " << Position << ", Length = " << Length << ")   ---   Status: Position = " << m_Position << ", Length = " << m_Length << ", Changing = " << m_Changing << std::endl;
#endif
    if(Position < m_Position)
    {
        if(Position + Length < m_Position)
        {
            // deleting completely before Buffer
            m_Position -= Length;
        }
        else
        {
            // deletion and Buffer intersect
            if(Position + Length < m_Position + m_Length)
            {
                // partly intersection
                m_Length = m_Position + m_Length - (Position + Length);
                for(auto Marker : m_Markers)
                {
                    if(m_Position + Marker->GetPosition() <= Position + Length)
                    {
                        Marker->SetPosition(0);
                    }
                    else
                    {
                        Marker->SetPosition(m_Position + Marker->GetPosition() - (Position + Length));
                    }
                }
            }
            else
            {
                // deletion covers Buffer
                m_Length = 0;
                for(auto Marker : m_Markers)
                {
                    Marker->SetPosition(0);
                }
            }
            m_Position = Position;
        }
    }
    else
    {
        // HERE: The deletion is after or at the begin of _this_ buffer
        //    => The position of _this_ buffer must not change
        if(Position < m_Position + m_Length)
        {
            // HERE: The deletion is NOT after the end of _this_ buffer
            if(Position + Length > m_Position + m_Length)
            {
                // HERE: The deletion overlaps the end of _this_ buffer
                //    => Reduce the size of _this_ buffer by the length of the overlapping
                //
                // Condition:
                //                  Position >= m_Position
                // =>  Position - m_Position >= 0
                // =>  this difference is greater than/or equal zero and thus safe
                m_Length = Position - m_Position;
            }
            else
            {
                // HERE: The deletion is completely inside _this_ buffer
                //    => Reduce the size of _this_ buffer by the size of the deletion
                //
                // Condition 1:
                //                  Position >= m_Position
                // =>  Position - m_Position >= 0
                //
                // Condition 2:
                //                    Position + Length <= m_Position + m_Length
                // =>  (Position - m_Position) + Length <= m_Length
                // =>                        X + Length <= m_Length
                // =>                          m_Length >= Length + X
                // =>  this difference is greater than/or equal zero and thus safe
                m_Length -= Length;
            }
        }
        else
        {
            // HERE: The deletion is after the end of _this_ buffer
            //    => Nothing to be done
        }
    }
    m_Begin = m_SupBuffer->m_Begin + m_Position;
    for(auto SubBuffer : m_SubBuffers)
    {
        SubBuffer->GetBuffer().m_ParentDataDeleted(Position - m_Position, Length);
    }
}

auto ARX::Buffer::m_ParentDataInserted(ARX::Buffer::size_type Position, ARX::Buffer::size_type Length) -> void
{
#ifdef DEBUG
	std::cerr << this << Indentation(this) << " ParentDataInserted(Position = " << Position << ", Length = " << Length << ")   ---   Status: Position = " << m_Position << ", Length = " << m_Length << ", Changing = " << m_Changing << std::endl;
#endif
    if((Position == m_Position) && (m_Changing == true))
    {
        // prepending
        m_Length += Length;
    }
    else
    {
        if(Position <= m_Position)
        {
            // data inserted; we are not prepending; inserting position is before this buffer
            m_Position += Length;
        }
        else
        {
            if((Position <= m_Position + m_Length) && (m_Changing == true))
            {
                m_Length += Length;
            }
        }
    }
    // data inserted; we are not prepending; inserting position is after the begin of this buffer
    // update markers
    for(auto Marker : m_Markers)
    {
        if(((m_Position + Marker->GetPosition() == Position) && (Marker->GetAlignment() == ARX::Buffer::Marker::Alignment::Right)) || (Marker->GetPosition() > Position))
        {
            // only if the data was inserted BEFORE or AT the marker position
            Marker->SetPosition(Marker->GetPosition() + Length);
        }
    }
    m_Begin = m_SupBuffer->m_Begin + m_Position;
    // NEXT: because, if we are changing there is an order, which must be followed.
    if(m_Changing == false)
    {
        for(auto SubBuffer : m_SubBuffers)
        {
            if(Position >= m_Position)
            {
                SubBuffer->GetBuffer().m_ParentDataInserted(Position - m_Position, Length);
            }
        }
    }
}

auto ARX::Buffer::m_ParentDataUpdated() -> void
{
#ifdef DEBUG
	std::cerr << this << Indentation(this) << " ParentDataUpdate()   ---   Status: Position = " << m_Position << ", Length = " << m_Length << ", Changing = " << m_Changing << '\n';
#endif
    m_Begin = m_SupBuffer->m_Begin + m_Position;
    for(auto SubBuffer : m_SubBuffers)
    {
        SubBuffer->GetBuffer().m_ParentDataUpdated();
    }
}

ARX::Buffer::value_type ARX::Buffer::operator[](size_type Index) const
{
	if(Index >= m_Length)
	{
		throw std::out_of_range("Trying to read after the end.");
	}

	return m_Begin[Index];
}

ARX::Buffer::reference ARX::Buffer::operator[](size_type Index)
{
	if(Index >= m_Length)
	{
		throw std::out_of_range("Trying to read after the end.");
	}

	return m_Begin[Index];
}

std::ostream & ARX::operator<<(std::ostream & OStream, const ARX::Buffer & Buffer)
{
	OStream.write(const_cast<char *>(reinterpret_cast<char const *>(Buffer.GetBegin())), Buffer.GetLength());
	
	return OStream;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Buffer::Marker                                                                          //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Buffer::Marker::Marker(ARX::Buffer const & Buffer, ARX::Buffer::size_type Position, ARX::Buffer::Marker::Alignment Alignment) :
	m_Buffer{&Buffer},
	m_Position{(m_Buffer->GetLength() > Position) ? (Position) : (m_Buffer->GetLength())},
	m_Alignment{Alignment}
{
	m_Buffer->m_Register(*this);
}

ARX::Buffer::Marker::~Marker()
{
	m_Buffer->m_Unregister(*this);
}

ARX::Buffer::size_type ARX::Buffer::Marker::GetPosition() const
{
	return m_Position;
}

void ARX::Buffer::Marker::SetPosition(ARX::Buffer::size_type Position)
{
	m_Position = (m_Buffer->GetLength() > Position) ? (Position) : (m_Buffer->GetLength());
}

ARX::Buffer::Marker::Alignment ARX::Buffer::Marker::GetAlignment() const
{
	return m_Alignment;
}

void ARX::Buffer::Marker::SetAlignment(ARX::Buffer::Marker::Alignment Alignment)
{
	m_Alignment = Alignment;
}

bool ARX::Buffer::Marker::IsValid() const
{
	return m_Buffer != nullptr;
}

void ARX::Buffer::Marker::m_InvalidateBuffer()
{
	m_Buffer = nullptr;
}
