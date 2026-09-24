/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <cassert>
#include <iostream>

#include <arx/buffer.h>
#include <arx/buffer_writer.h>
#include <arx/reference.h>
#include <arx/structure.h>

#include "io.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Structure                                                                                //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Structure::Structure(ARX::Item & Item) :
	m_Item{Item}
{
}

auto ARX::Structure::Add(std::uint32_t ItemIdentifier, std::string const & Relation) -> bool
{
	if((Relation == "reference") || (ItemIdentifier == g_InvalidItemIdentifier))
	{
		return false;
	}
	
	auto RelationIterator = m_Relations.find(Relation);
	
	if(RelationIterator == m_Relations.end())
	{
		RelationIterator = m_Relations.insert(std::make_pair(Relation, ARX::Structure::Relation(*this, Relation))).first;
	}
	RelationIterator->second.Add(ItemIdentifier);
	
	return true;
}

auto ARX::Structure::Delete(std::uint32_t ItemIdentifier, std::string const & Relation) -> bool
{
	if((Relation == "reference") || (ItemIdentifier == g_InvalidItemIdentifier))
	{
		return false;
	}
	
	auto RelationIterator = m_Relations.find(Relation);
	
	if(RelationIterator == m_Relations.end())
	{
		return false;
	}
	
	return RelationIterator->second.Delete(ItemIdentifier);
}

auto ARX::Structure::GetRelation(std::string const & Relation) const -> ARX::Structure::Relation const &
{
	auto RelationIterator = m_Relations.find(Relation);
	
	if(RelationIterator == m_Relations.end())
	{
		throw std::runtime_error{"Relation \"" + Relation + "\" not found."};
	}
	else
	{
		return RelationIterator->second;
	}
}

auto ARX::Structure::GetRelation(std::string const & Relation) -> ARX::Structure::Relation &
{
	auto RelationIterator = m_Relations.find(Relation);
	
	if(RelationIterator == m_Relations.end())
	{
		throw std::runtime_error{"Relation \"" + Relation + "\" not found."};
	}
	else
	{
		return RelationIterator->second;
	}
}

auto ARX::Structure::size() const -> ARX::Structure::size_type
{
	return m_Relations.size();
}

auto ARX::Structure::begin() -> ARX::Structure::iterator
{
	return ARX::Structure::iterator{m_Relations.begin()};
}

auto ARX::Structure::end() -> ARX::Structure::iterator
{
	return ARX::Structure::iterator{m_Relations.end()};
}

auto ARX::Structure::begin() const -> ARX::Structure::const_iterator
{
	return ARX::Structure::const_iterator{m_Relations.begin()};
}

auto ARX::Structure::end() const -> ARX::Structure::const_iterator
{
	return ARX::Structure::const_iterator{m_Relations.end()};
}

auto ARX::Structure::m_RemoveRelation(ARX::Structure::Relation * Relation) -> void
{
    std::erase_if(m_Relations, [Relation](auto const & Item)
                               {
                                   return std::addressof(Item.second) == Relation;
                               });
}

auto ARX::Structure::HasRelation(std::string const & Relation) const -> bool
{
	return m_Relations.find(Relation) != m_Relations.end();
}

auto ARX::Structure::m_ReadFromStream(std::uint32_t StructureDataLength, std::istream & IStream) -> void
{
	while(StructureDataLength > 0)
	{
		auto Relation = ARX::ReadStringWithTermination(IStream);
		
		assert(IStream);
		StructureDataLength -= Relation.length() + 1;
		
		auto ItemCount = ARX::ReadUnsignedInteger32Bit(IStream);
		
		assert(IStream);
		StructureDataLength -= 4;
		while(ItemCount > 0)
		{
			auto ItemIdentifier = ARX::ReadUnsignedInteger32Bit(IStream);
			
			assert(IStream);
			StructureDataLength -= 4;
			Add(ItemIdentifier, Relation);
			--ItemCount;
		}
	}
}

auto ARX::Structure::m_WriteToBuffer(ARX::Buffer & Buffer) const -> void
{
	auto BufferWriter = ARX::BufferWriter{Buffer};
    
    for(auto & [RelationName, Relation] : m_Relations)
    {
		BufferWriter << RelationName;
		BufferWriter << ARX::ToNetworkByteOrder(Relation.size());
		for(auto Reference : Relation)
        {
			BufferWriter << ARX::ToNetworkByteOrder(Reference.GetItemIdentifier());
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Structure::iterator                                                                     //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Structure::iterator::iterator(std::map<std::string, ARX::Structure::Relation>::iterator Iterator) :
	m_Iterator{Iterator}
{
}

auto ARX::Structure::iterator::operator++() -> ARX::Structure::iterator &
{
	++m_Iterator;
	
	return *this;
}

auto ARX::Structure::iterator::operator*() -> ARX::Structure::Relation &
{
	return m_Iterator->second;
}

auto ARX::Structure::iterator::operator*() const -> ARX::Structure::Relation const &
{
	return m_Iterator->second;
}

auto ARX::Structure::iterator::operator->() -> ARX::Structure::Relation *
{
	return std::addressof(m_Iterator->second);
}

auto ARX::Structure::iterator::operator==(ARX::Structure::iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Structure::iterator::operator!=(ARX::Structure::iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}

auto ARX::Structure::iterator::operator==(ARX::Structure::const_iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Structure::iterator::operator!=(ARX::Structure::const_iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Structure::const_iterator                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Structure::const_iterator::const_iterator(std::map<std::string, ARX::Structure::Relation>::const_iterator Iterator) :
	m_Iterator{Iterator}
{
}

ARX::Structure::const_iterator::const_iterator(ARX::Structure::iterator Iterator) :
	m_Iterator{Iterator.m_Iterator}
{
}

auto ARX::Structure::const_iterator::operator++() -> ARX::Structure::const_iterator &
{
	++m_Iterator;
	
	return *this;
}

auto ARX::Structure::const_iterator::operator*() const -> ARX::Structure::Relation const &
{
	return m_Iterator->second;
}

auto ARX::Structure::const_iterator::operator->() -> ARX::Structure::Relation const *
{
	return std::addressof(m_Iterator->second);
}

auto ARX::Structure::const_iterator::operator==(ARX::Structure::const_iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Structure::const_iterator::operator!=(ARX::Structure::const_iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}

auto ARX::operator<<(ARX::Buffer & Buffer, ARX::Structure const & Structure) -> ARX::Buffer &
{
	Structure.m_WriteToBuffer(Buffer);
	
	return Buffer;
}
