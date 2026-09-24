/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <cassert>

#include <arx/item.h>
#include <arx/archive.h>

#include "reference_core.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::ReferenceCore                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////////
ARX::ReferenceCore::~ReferenceCore()
{
	assert(m_ReferenceCount == 0);
}

auto ARX::ReferenceCore::Create(ARX::Item & Item) -> ARX::ReferenceCore *
{
	auto ReferenceCore = new ARX::ReferenceCore{};
	
	ReferenceCore->m_Item = &Item;
	ReferenceCore->m_ItemIdentifier = Item.GetIdentifier();
	ReferenceCore->m_ReferenceCount = 1;
	ReferenceCore->m_Archive = Item.GetArchive();
	
	return ReferenceCore;
}

auto ARX::ReferenceCore::Create(std::uint32_t ItemIdentifier, ARX::Archive * Archive) -> ARX::ReferenceCore *
{
	auto ReferenceCore = new ARX::ReferenceCore{};
	
	ReferenceCore->m_Item = nullptr;
	ReferenceCore->m_ItemIdentifier = ItemIdentifier;
	ReferenceCore->m_ReferenceCount = 1;
	ReferenceCore->m_Archive = Archive;
	
	return ReferenceCore;
}

auto ARX::ReferenceCore::Create(ARX::ReferenceCore * ReferenceCore) -> ARX::ReferenceCore *
{
	ReferenceCore->m_ReferenceCount++;
	
	return ReferenceCore;
}

auto ARX::ReferenceCore::Release(ARX::ReferenceCore * ReferenceCore) -> bool
{
	assert(ReferenceCore->m_ReferenceCount != 0);
	ReferenceCore->m_ReferenceCount--;
	if(ReferenceCore->m_ReferenceCount == 0)
	{
		delete ReferenceCore;
		
		return true;
	}
	else
	{
		if((ReferenceCore->m_Item == nullptr) && (ReferenceCore->m_Archive != nullptr) && (ReferenceCore->m_ReferenceCount == 1))
		{
			ReferenceCore->m_Archive->ReleaseReferenceCore(ReferenceCore);
			
			return true;
		}
		else
		{
			return false;
		}
	}
}

auto ARX::ReferenceCore::GetItemIdentifier() const -> std::uint32_t
{
	return m_ItemIdentifier;
}

auto ARX::ReferenceCore::GetItem() -> ARX::Item *
{
	return m_Item;
}

auto ARX::ReferenceCore::GetItem() const -> ARX::Item const *
{
	return m_Item;
}

auto ARX::ReferenceCore::GetReferenceCount() const -> std::uint32_t
{
	return m_ReferenceCount;
}

auto ARX::ReferenceCore::Resolve(ARX::Item & Item) -> void
{
	if(GetItem() != nullptr)
	{
		throw std::runtime_error{"ARX::ReferenceCore::Resolve: Trying to resolve a resolved reference."};
	}
	if(Item.GetIdentifier() != GetItemIdentifier())
	{
		throw std::runtime_error{"ARX::ReferenceCore::Resolve: Trying to resolve a reference with different unique ID."};
	}
	m_Item = std::addressof(Item);
}

auto ARX::ReferenceCore::Unresolve() -> void
{
	if(GetItem() == nullptr)
	{
		throw std::runtime_error{"ARX::ReferenceCore::Unresolve: Trying to unresolve an unresolved reference."};
	}
	m_Item = nullptr;
}

auto ARX::ReferenceCore::DecoupleFromArchive() -> void
{
	m_Archive = nullptr;
}
