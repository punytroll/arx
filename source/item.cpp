/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <cassert>

#include <arx/item.h>
#include <arx/archive.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Item                                                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////
auto ARX::Item::Create() -> ARX::Item *
{
	return ARX::Item::Create(nullptr, g_InvalidItemIdentifier);
}

auto ARX::Item::Create(std::uint32_t Identifier) -> ARX::Item *
{
	return ARX::Item::Create(nullptr, Identifier);
}

auto ARX::Item::Create(ARX::Archive & Archive) -> ARX::Item *
{
	return ARX::Item::Create(&Archive);
}

auto ARX::Item::Create(ARX::Archive & Archive, std::uint32_t Identifier) -> ARX::Item *
{
	return ARX::Item::Create(&Archive, Identifier);
}

auto ARX::Item::Create(ARX::Archive * Archive) -> ARX::Item *
{
	return ARX::Item::Create(Archive, g_InvalidItemIdentifier);
}

auto ARX::Item::Create(ARX::Archive * Archive, std::uint32_t Identifier) -> ARX::Item *
{
	auto Result = new ARX::Item{};
	
	Result->SetIdentifier(Identifier);
	if(Archive != nullptr)
	{
		Archive->Register(Result);
	}
	
	return Result;
}

auto ARX::Item::Delete(ARX::Item * Item) -> void
{
	if(Item->m_Archive != nullptr)
	{
		Item->m_Archive->Unregister(Item);
		Item->m_Archive = nullptr;
	}
	delete Item;
}

ARX::Item::Item() :
	m_Structure{*this}
{
}

ARX::Item::~Item()
{
	assert(m_Archive == nullptr);
}

auto ARX::Item::GetIdentifier() const -> std::uint32_t
{
	return m_Identifier;
}

auto ARX::Item::SetIdentifier(std::uint32_t Identifier) -> void
{
	if(m_Archive == nullptr)
	{
		m_Identifier = Identifier;
	}
	else
	{
		// save the archive as it will be set to null in UnregisterItem.
		auto Archive = m_Archive;
		
		try
		{
			Archive->Unregister(this);
		}
		catch(std::invalid_argument & Exception)
		{
		}
		
		auto OldIdentifier = m_Identifier;
		
		m_Identifier = Identifier;
		try
		{
			Archive->Register(this);
		}
		catch(ARX::id_not_unique & Exception)
		{
			m_Identifier = OldIdentifier;
			m_Archive->Register(this);
			throw;
		}
	}
}

auto ARX::Item::GetName() const -> std::string const &
{
	return m_Name;
}

auto ARX::Item::SetName(std::string const & Name) -> void
{
	m_Name = Name;
}

auto ARX::Item::GetType() const -> std::uint32_t
{
	return m_Type;
}

auto ARX::Item::SetType(std::uint32_t Type) -> void
{
	m_Type = Type;
}

auto ARX::Item::GetSubType() const -> std::uint32_t
{
	return m_SubType;
}

auto ARX::Item::SetSubType(std::uint32_t SubType) -> void
{
	m_SubType = SubType;
}

auto ARX::Item::GetMajorVersionNumber() const -> std::uint8_t
{
	return m_MajorVersionNumber;
}

auto ARX::Item::GetMinorVersionNumber() const -> std::uint8_t
{
	return m_MinorVersionNumber;
}

auto ARX::Item::GetRevisionNumber() const -> std::uint8_t
{
	return m_RevisionNumber;
}

auto ARX::Item::GetCandidateNumber() const -> std::uint8_t
{
	return m_CandidateNumber;
}

auto ARX::Item::SetVersionNumbers(std::uint8_t MajorVersionNumber, std::uint8_t MinorVersionNumber, std::uint8_t RevisionNumber, std::uint8_t CandidateNumber) -> void
{
	m_MajorVersionNumber = MajorVersionNumber;
	m_MinorVersionNumber = MinorVersionNumber;
	m_RevisionNumber = RevisionNumber;
	m_CandidateNumber = CandidateNumber;
}

auto ARX::Item::GetArchive() const -> ARX::Archive *
{
	return m_Archive;
}

auto ARX::Item::GetStructure() -> ARX::Structure &
{
	return m_Structure;
}

auto ARX::Item::GetStructure() const -> ARX::Structure const &
{
	return m_Structure;
}

auto ARX::Item::m_Fetch(std::uint32_t Offset, std::uint32_t Length) -> bool
{
	if(m_Archive != nullptr)
	{
		return m_Archive->Fetch(Offset, Length, this);
	}
	else
	{
		return Length == 0;
	}
}
