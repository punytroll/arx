/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <cassert>

#include <arx/reference.h>

#include "reference_core.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Reference                                                                                //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Reference::Reference(ARX::Item & Item) :
	m_ReferenceCore{ARX::ReferenceCore::Create(Item)}
{
}

ARX::Reference::Reference(std::uint32_t ItemIdentifier, ARX::Archive * Archive) :
	m_ReferenceCore{ARX::ReferenceCore::Create(ItemIdentifier, Archive)}
{
}

ARX::Reference::Reference(ARX::Reference const & Reference) :
	m_ReferenceCore{nullptr}
{
	m_SetCore(Reference.m_ReferenceCore);
}

ARX::Reference::~Reference()
{
	m_SetCore(nullptr);
}

auto ARX::Reference::GetItemIdentifier() const -> std::uint32_t
{
	return m_ReferenceCore->GetItemIdentifier();
}

auto ARX::Reference::GetItem() -> ARX::Item *
{
	return m_ReferenceCore->GetItem();
}

auto ARX::Reference::GetItem() const -> ARX::Item const *
{
	return m_ReferenceCore->GetItem();
}

auto ARX::Reference::GetReferenceCount() const -> std::uint32_t
{
	return m_ReferenceCore->GetReferenceCount();
}

auto ARX::Reference::Resolve(ARX::Item & Item) -> void
{
	m_ReferenceCore->Resolve(Item);
}

auto ARX::Reference::Unresolve() -> void
{
	m_ReferenceCore->Unresolve();
}

auto ARX::Reference::Attach(ARX::Reference & Reference) -> bool
{
	if((Reference.GetItemIdentifier() != GetItemIdentifier()) || (Reference.GetItem() != nullptr))
	{
		return false;
	}
	Reference.m_SetCore(m_ReferenceCore);
	
	return true;
}

auto ARX::Reference::Detach() -> bool
{
	auto ReferenceCore = ARX::ReferenceCore::Create(GetItemIdentifier());
	
	m_SetCore(ReferenceCore);
	// releasing the local pointer
	ARX::ReferenceCore::Release(ReferenceCore);
	
	return true;
}

auto ARX::Reference::m_DecoupleFromArchive() -> void
{
	m_ReferenceCore->DecoupleFromArchive();
}

auto ARX::Reference::m_SetCore(ARX::ReferenceCore * ReferenceCore) -> void
{
	if(m_ReferenceCore != nullptr)
	{
		ARX::ReferenceCore::Release(m_ReferenceCore);
		m_ReferenceCore = nullptr;
	}
	if(ReferenceCore != nullptr)
	{
		m_ReferenceCore = ARX::ReferenceCore::Create(ReferenceCore);
	}
}
