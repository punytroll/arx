/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <arx/archive.h>
#include <arx/item.h>
#include <arx/reference.h>
#include <arx/structure.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Structure::Relation                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Structure::Relation::Relation(ARX::Structure & Structure, std::string const & Name) :
	m_Structure{Structure},
	m_Name{Name}
{
}

auto ARX::Structure::Relation::Add(std::uint32_t ItemIdentifier) -> void
{
	if(m_Structure.m_Item.GetArchive() == nullptr)
	{
		m_References.insert(std::make_pair(ItemIdentifier, ARX::Reference(ItemIdentifier)));
	}
	else
	{
		m_References.insert(std::make_pair(ItemIdentifier, m_Structure.m_Item.GetArchive()->GetReference(ItemIdentifier)));
	}
}

auto ARX::Structure::Relation::Delete(std::uint32_t ItemIdentifier) -> bool
{
	auto Iterator = m_References.lower_bound(ItemIdentifier);
	
	if(Iterator == m_References.end())
	{
		return false;
	}
	m_References.erase(Iterator);
	if(m_References.size() == 0)
	{
		m_Structure.m_RemoveRelation(this);
	}
	
	return true;
}

auto ARX::Structure::Relation::GetItems(std::string const & Name) -> std::list<ARX::Item *>
{
	auto Result = std::list<ARX::Item *>{};
	
	for(auto & [ItemIdentifier, Reference] : m_References)
	{
		if((Reference.GetItem() != nullptr) && (Reference.GetItem()->GetName() == Name))
		{
			Result.push_back(Reference.GetItem());
		}
	}
	
	return Result;
}

auto ARX::Structure::Relation::size() const -> ARX::Structure::Relation::size_type
{
	return m_References.size();
}

auto ARX::Structure::Relation::begin() -> ARX::Structure::Relation::iterator
{
	return ARX::Structure::Relation::iterator{m_References.begin()};
}

auto ARX::Structure::Relation::end() -> ARX::Structure::Relation::iterator
{
	return ARX::Structure::Relation::iterator{m_References.end()};
}

auto ARX::Structure::Relation::begin() const -> ARX::Structure::Relation::const_iterator
{
	return ARX::Structure::Relation::const_iterator{m_References.begin()};
}

auto ARX::Structure::Relation::end() const -> ARX::Structure::Relation::const_iterator
{
	return ARX::Structure::Relation::const_iterator{m_References.end()};
}

auto ARX::Structure::Relation::GetName() const -> std::string const &
{
	return m_Name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Structure::Relation::iterator                                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Structure::Relation::iterator::iterator(std::multimap<std::uint32_t, ARX::Reference>::iterator Iterator) :
	m_Iterator{Iterator}
{
}

auto ARX::Structure::Relation::iterator::operator++() -> ARX::Structure::Relation::iterator &
{
	++m_Iterator;
	
	return *this;
}

auto ARX::Structure::Relation::iterator::operator*() -> ARX::Reference &
{
	return m_Iterator->second;
}

auto ARX::Structure::Relation::iterator::operator*() const -> ARX::Reference const &
{
	return m_Iterator->second;
}

auto ARX::Structure::Relation::iterator::operator->() -> ARX::Reference *
{
	return std::addressof(m_Iterator->second);
}

auto ARX::Structure::Relation::iterator::operator==(ARX::Structure::Relation::iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Structure::Relation::iterator::operator!=(ARX::Structure::Relation::iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}

auto ARX::Structure::Relation::iterator::operator==(ARX::Structure::Relation::const_iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Structure::Relation::iterator::operator!=(ARX::Structure::Relation::const_iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Structure::Relation::const_iterator                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Structure::Relation::const_iterator::const_iterator(std::multimap<std::uint32_t, ARX::Reference>::const_iterator Iterator) :
	m_Iterator{Iterator}
{
}

ARX::Structure::Relation::const_iterator::const_iterator(ARX::Structure::Relation::iterator Iterator) :
	m_Iterator{Iterator.m_Iterator}
{
}

auto ARX::Structure::Relation::const_iterator::operator++() -> ARX::Structure::Relation::const_iterator &
{
	++m_Iterator;
	
	return *this;
}

auto ARX::Structure::Relation::const_iterator::operator*() const -> ARX::Reference const &
{
	return m_Iterator->second;
}

auto ARX::Structure::Relation::const_iterator::operator->() -> ARX::Reference const *
{
	return std::addressof(m_Iterator->second);
}

auto ARX::Structure::Relation::const_iterator::operator==(ARX::Structure::Relation::const_iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Structure::Relation::const_iterator::operator!=(ARX::Structure::Relation::const_iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}
