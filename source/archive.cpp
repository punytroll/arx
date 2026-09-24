/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <arx/archive.h>
#include <arx/buffer_writer.h>
#include <arx/item.h>
#include <arx/reference.h>

#include "archive_file.h"
#include "reference_core.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Archive                                                                                  //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Archive::Archive()
{
	srand(time(0));
}

ARX::Archive::~Archive()
{
	Close();
}

auto ARX::Archive::Load(std::string const & FilePath) -> bool
{
	m_IStream = new std::ifstream{FilePath.c_str()};
	if(m_IStream->fail())
	{
		return false;
	}
	// this position I chose after only a few looks at the file => prove me wrong ;)
	Close();
	
	auto ArchiveHeader =  ARX::ArchiveHeader{};
	
	(*m_IStream) >> ArchiveHeader;
	if((ArchiveHeader.MajorVersionNumber == 2) && (ArchiveHeader.MinorVersionNumber == 1) && (ArchiveHeader.RevisionNumber == 0) && (ArchiveHeader.CandidateNumber == 0))
	{
		m_Read_2_1_0_0(ArchiveHeader.NumberOfItems);
		m_RootItem = GetItem(ArchiveHeader.RootItemIdentifier);
	}
	else
	{
		std::cerr << "Unknown ARX archive format: version = " << static_cast<std::uint32_t>(ArchiveHeader.MajorVersionNumber) << '.' << static_cast<std::uint32_t>(ArchiveHeader.MinorVersionNumber) << '.' << static_cast<std::uint32_t>(ArchiveHeader.RevisionNumber) << '.' << static_cast<std::uint32_t>(ArchiveHeader.CandidateNumber) << '.' << std::endl;
		throw ARX::bad_file_format{FilePath};
	}
	
	return true;
}

auto ARX::Archive::Close() -> void
{
	while(m_Items.begin() != m_Items.end())
	{
		auto Item = m_Items.begin()->second;
		
		Unregister(Item);
		ARX::Item::Delete(Item);
	}
	m_RootItem = nullptr;
	
	auto Iterator = m_References.begin();
	
	while(Iterator != m_References.end())
	{
		Iterator->second.Unresolve();
		Iterator = m_References.erase(Iterator);
	}
}

auto ARX::Archive::Register(ARX::Item * Item) -> void
{
	if(Item == nullptr)
	{
		throw std::invalid_argument{"Item"};
	}
	if(Item->m_Archive == nullptr)
	{
		Item->m_Archive = this;
	}
	if(Item->m_Archive != this)
	{
		throw std::invalid_argument{"Item"};
	}
	if(Item->GetIdentifier() == g_InvalidItemIdentifier)
	{
		auto Iterator = std::map<std::uint32_t, ARX::Item *>::iterator{};
		auto NewIdentifier = std::uint32_t{};
		
		do
		{
			NewIdentifier = rand();
			Iterator = m_Items.find(NewIdentifier);
		} while((Iterator != m_Items.end()) || (NewIdentifier == g_InvalidItemIdentifier));
		Item->SetIdentifier(NewIdentifier);
	}
	else
	{
		auto ItemIterator = m_Items.find(Item->GetIdentifier());
		
		if(ItemIterator != m_Items.end())
		{
			throw ARX::id_not_unique{std::to_string(Item->GetIdentifier())};
		}
		m_Items[Item->GetIdentifier()] = Item;
		
		auto ReferenceIterator = m_References.find(Item->GetIdentifier());
		
		if(ReferenceIterator == m_References.end())
		{
			m_References.insert(std::make_pair(Item->GetIdentifier(), ARX::Reference(*Item)));
		}
		else
		{
			ReferenceIterator->second.Resolve(*Item);
		}
		
		/** @todo the following is the brute force implementation. It can be done much faster with a merge sort **/
		auto RelationIterator = Item->GetStructure().begin();
		auto RelationEnd = Item->GetStructure().end();
		
		while(RelationIterator != RelationEnd)
		{
			auto ReferenceIterator = RelationIterator->begin();
			auto ReferenceEnd = RelationIterator->end();
			
			while(ReferenceIterator != ReferenceEnd)
			{
				// use GetReference() to create or retrieve the Reference in the archive's Reference map and then attach the Item's Reference to it
				auto Reference = ARX::Reference{GetReference(ReferenceIterator->GetItemIdentifier())};
				
				Reference.Attach(*ReferenceIterator);
				++ReferenceIterator;
			}
			++RelationIterator;
		}
	}
}

auto ARX::Archive::Unregister(ARX::Item * Item) -> void
{
	if(Item->m_Archive != this)
	{
		throw std::invalid_argument{"Item"};
	}
	
	auto ItemIterator = m_Items.find(Item->GetIdentifier());
	auto ReferenceIterator = m_References.find(Item->GetIdentifier());
	
	if((ItemIterator == m_Items.end()) || (ReferenceIterator == m_References.end()))
	{
		throw std::invalid_argument{"Item"};
	}
	
	// before removing the Item from the Archive we correct its references
	auto RelationIterator = Item->GetStructure().begin();
	auto RelationEnd = Item->GetStructure().end();
	
	while(RelationIterator != RelationEnd)
	{
		auto ReferenceIterator = RelationIterator->begin();
		auto ReferenceEnd = RelationIterator->end();
		
		while(ReferenceIterator != ReferenceEnd)
		{
			ReferenceIterator->Detach();
			++ReferenceIterator;
		}
		++RelationIterator;
	}
	// now we can remove the Item
	m_Items.erase(ItemIterator);
	Item->m_Archive = nullptr;
	ReferenceIterator->second.Unresolve();
	ReferenceIterator->second.m_DecoupleFromArchive();
	m_References.erase(ReferenceIterator);
}

auto ARX::Archive::SetRootItem(ARX::Item * Item) -> void
{
	if((Item != nullptr) && (Item->m_Archive != this))
	{
		throw std::invalid_argument{"Item"};
	}
	m_RootItem = Item;
}

auto ARX::Archive::GetItem(std::uint32_t ItemIdentifier) const -> ARX::Item const *
{
	if(ItemIdentifier == g_InvalidItemIdentifier)
	{
		return nullptr;
	}
	
	auto ItemIterator = m_Items.find(ItemIdentifier);
	
	if(ItemIterator == m_Items.end())
	{
		return nullptr;
	}
	
	return ItemIterator->second;
}

auto ARX::Archive::GetItem(std::uint32_t ItemIdentifier) -> ARX::Item *
{
	if(ItemIdentifier == g_InvalidItemIdentifier)
	{
		return nullptr;
	}
	
	auto ItemIterator(m_Items.find(ItemIdentifier));
	
	if(ItemIterator == m_Items.end())
	{
		return nullptr;
	}
	
	return ItemIterator->second;
}

auto ARX::Archive::GetItem(std::string Path) -> ARX::Item *
{
	if(Path[0] != '/')
	{
		return nullptr;
	}
	
	auto ChildRelation = std::string{};
	auto Item = GetRootItem();
	
	while((Item != nullptr) && ((Path != "/") || (Path == "")))
	{
		auto PathSeparatorIndex = Path.find('/', 1);
		auto ChildName = Path.substr(1, PathSeparatorIndex - 1);
		
		if(PathSeparatorIndex == std::string::npos)
		{
			Path = "/";
		}
		else
		{
			Path = Path.substr(PathSeparatorIndex);
		}
		
		auto RelationSeparatorIndex = ChildName.find("::");
		
		if(RelationSeparatorIndex != std::string::npos)
		{
			ChildRelation = ChildName.substr(0, RelationSeparatorIndex);
			ChildName = ChildName.substr(RelationSeparatorIndex + 2);
		}
		else
		{
			ChildRelation = "child";
		}
		if(Item->GetStructure().HasRelation(ChildRelation) == false)
		{
			return nullptr;
		}
		else
		{
			auto & Relation = Item->GetStructure().GetRelation(ChildRelation);
			auto ReferenceIterator = Relation.begin();
			
			for(; ReferenceIterator != Relation.end(); ++ReferenceIterator)
			{
				if((ReferenceIterator->GetItem() != nullptr) && (ReferenceIterator->GetItem()->GetName() == ChildName))
				{
					Item = ReferenceIterator->GetItem();
					
					break;
				}
			}
			if(ReferenceIterator == Relation.end())
			{
				return nullptr;
			}
		}
	}
	
	return Item;
}

auto ARX::Archive::GetItem(std::string Path) const -> ARX::Item const *
{
	if(Path[0] != '/')
	{
		return nullptr;
	}
	
	auto ChildRelation = std::string{};
	auto Item = GetRootItem();
	
	while((Item != nullptr) && ((Path != "/") || (Path == "")))
	{
		auto PathSeparatorIndex = Path.find('/', 1);
		auto ChildName = Path.substr(1, PathSeparatorIndex - 1);
		
		if(PathSeparatorIndex == std::string::npos)
		{
			Path = "/";
		}
		else
		{
			Path = Path.substr(PathSeparatorIndex);
		}
		
		auto RelationSeparatorIndex = ChildName.find("::");
		
		if(RelationSeparatorIndex != std::string::npos)
		{
			ChildRelation = ChildName.substr(0, RelationSeparatorIndex);
			ChildName = ChildName.substr(RelationSeparatorIndex + 2);
		}
		else
		{
			ChildRelation = "child";
		}
		if(Item->GetStructure().HasRelation(ChildRelation) == false)
		{
			return nullptr;
		}
		else
		{
			auto & Relation = Item->GetStructure().GetRelation(ChildRelation);
			auto ReferenceIterator = Relation.begin();
			
			for(; ReferenceIterator != Relation.end(); ++ReferenceIterator)
			{
				if((ReferenceIterator->GetItem() != nullptr) && (ReferenceIterator->GetItem()->GetName() == ChildName))
				{
					Item = ReferenceIterator->GetItem();
					
					break;
				}
			}
			if(ReferenceIterator == Relation.end())
			{
				return nullptr;
			}
		}
	}
	
	return Item;
}

auto ARX::Archive::GetRootItem() -> ARX::Item *
{
	return m_RootItem;
}

auto ARX::Archive::GetRootItem() const -> ARX::Item const *
{
	return m_RootItem;
}

auto ARX::Archive::GetReference(std::uint32_t ItemIdentifier) -> ARX::Reference
{
	auto ReferenceIterator = m_References.find(ItemIdentifier);
	
	if(ReferenceIterator == m_References.end())
	{
		return m_References.insert(std::make_pair(ItemIdentifier, ARX::Reference(ItemIdentifier, this))).first->second;
	}
	else
	{
		return ReferenceIterator->second;
	}
}

auto ARX::Archive::ReleaseReferenceCore(ARX::ReferenceCore * ReferenceCore) -> void
{
	auto ReferenceIterator = m_References.find(ReferenceCore->GetItemIdentifier());
	
	if(ReferenceIterator == m_References.end())
	{
		throw std::runtime_error{"ARX::Archive::vReleaseReference: Reference not found in the reference map."};
	}
	m_References.erase(ReferenceIterator);
}

auto ARX::Archive::m_Read_2_1_0_0(std::uint32_t ItemCount) -> void
{
	auto ItemIndex = 0UL;
	
	while(++ItemIndex <= ItemCount)
	{
        auto ItemHeader = ARX::ItemHeader{};
        
		(*m_IStream) >> ItemHeader;
		
		auto Item = ARX::Item::Create(this, ItemHeader.Identifier);
		
		Item->SetType(ItemHeader.Type);
		Item->SetSubType(ItemHeader.SubType);
		Item->SetVersionNumbers(ItemHeader.MajorVersionNumber, ItemHeader.MinorVersionNumber, ItemHeader.RevisionNumber, ItemHeader.CandidateNumber);
		
		auto Name = std::vector<char>(ItemHeader.NameLength + 1);
		
		m_IStream->read(Name.data(), ItemHeader.NameLength + 1);
		Name[ItemHeader.NameLength] = '\0';
		Item->SetName(Name.data());
		Item->GetStructure().m_ReadFromStream(ItemHeader.StructureLength, *m_IStream);
		Item->SetFetchInformation(m_IStream->tellg(), static_cast<ARX::Data::Compression>(ItemHeader.DataCompressionType), ItemHeader.DataDecompressedLength, ItemHeader.DataCompressedLength);
		m_IStream->seekg((ItemHeader.DataCompressionType == 0) ? (ItemHeader.DataDecompressedLength) : (ItemHeader.DataCompressedLength), std::ios_base::cur);
	}
}

auto ARX::Archive::Fetch(std::uint32_t Offset, std::uint32_t Length, ARX::Buffer * Buffer) -> bool
{
	m_IStream->seekg(Offset, std::ios_base::beg);
	Buffer->SetLength(0);
	
	auto BufferWriter = ARX::BufferWriter{*Buffer};
	
	BufferWriter << std::make_pair(static_cast<ARX::Buffer::size_type>(Length), reinterpret_cast<std::istream *>(m_IStream));
	
	return true;
}

auto ARX::Archive::Save(std::string const & FilePath, bool AutoCompress) -> void
{
    auto TemporaryFilePath = std::filesystem::temp_directory_path() / ("ARX" + std::to_string(time(0)));
	auto OStream = std::ofstream{TemporaryFilePath};
	
	if(!OStream)
	{
		throw ARX::file_error{TemporaryFilePath};
	}
	
	ARX::ArchiveHeader ArchiveHeader;
	
	ArchiveHeader.MajorVersionNumber = 2;
	ArchiveHeader.MinorVersionNumber = 1;
	ArchiveHeader.RevisionNumber = 0;
	ArchiveHeader.CandidateNumber = 0;
	ArchiveHeader.RootItemIdentifier = ((m_RootItem != nullptr) ? (m_RootItem->GetIdentifier()) : (g_InvalidItemIdentifier));
	ArchiveHeader.NumberOfItems = m_Items.size();
	OStream << ArchiveHeader;
	for(auto const & [ItemIdentifier, Item] : m_Items)
	{
		if(Item->IsFetched() == false)
		{
			Item->Fetch();
		}
		if((Item->IsFetched() == true) && (Item->IsCompressed() == false) && (Item->GetDecompressedLength() > 32) && (AutoCompress == true))
		{
			Item->Compress();
		}
		
		ARX::ItemHeader ItemHeader;
		
		ItemHeader.Identifier = Item->GetIdentifier();
		ItemHeader.Type = Item->GetType();
		ItemHeader.SubType = Item->GetSubType();
		ItemHeader.MajorVersionNumber = Item->GetMajorVersionNumber();
		ItemHeader.MinorVersionNumber = Item->GetMinorVersionNumber();
		ItemHeader.RevisionNumber = Item->GetRevisionNumber();
		ItemHeader.CandidateNumber = Item->GetCandidateNumber();
		ItemHeader.DataCompressionType = Item->GetCompression();
		ItemHeader.NameLength = Item->GetName().length();
		ItemHeader.DataDecompressedLength = Item->GetDecompressedLength();
		ItemHeader.DataCompressedLength = Item->GetCompressedLength();
		
		ARX::Data StructureBuffer;
		
		StructureBuffer << Item->GetStructure();
		ItemHeader.StructureLength = StructureBuffer.GetLength();
		OStream << ItemHeader << Item->GetName() << '\0' << StructureBuffer << *Item;
	}
	OStream.close();
	
	auto ChildPID = 0;
	auto Return = 0;
	
	if((ChildPID = fork()) == 0)
	{
		execlp("mv", "mv", "--force", TemporaryFilePath, FilePath.c_str(), reinterpret_cast<char *>(0));
	}
	waitpid(ChildPID, &Return, 0);
}

auto ARX::Archive::begin() -> ARX::Archive::iterator
{
	return ARX::Archive::iterator{m_Items.begin()};
}

auto ARX::Archive::end() -> ARX::Archive::iterator
{
	return ARX::Archive::iterator{m_Items.end()};
}

auto ARX::Archive::begin() const -> ARX::Archive::const_iterator
{
	return ARX::Archive::const_iterator{m_Items.begin()};
}

auto ARX::Archive::end() const -> ARX::Archive::const_iterator
{
	return ARX::Archive::const_iterator{m_Items.end()};
}

auto ARX::Archive::size() const -> ARX::Archive::size_type
{
	return m_Items.size();
}

auto ARX::Archive::GetNumberOfReferences() const -> std::uint32_t
{
	return m_References.size();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Archive::iterator                                                                        //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Archive::iterator::iterator(std::map<std::uint32_t, ARX::Item *>::iterator Iterator) :
	m_Iterator{Iterator}
{
}

ARX::Archive::iterator::~iterator()
{
}

auto ARX::Archive::iterator::operator++() -> ARX::Archive::iterator &
{
	++m_Iterator;

	return *this;
}

auto ARX::Archive::iterator::operator*() -> ARX::Item *
{
	return m_Iterator->second;
}

auto ARX::Archive::iterator::operator*() const -> ARX::Item const *
{
	return m_Iterator->second;
}

auto ARX::Archive::iterator::operator->() -> ARX::Item *
{
	return m_Iterator->second;
}

auto ARX::Archive::iterator::operator==(ARX::Archive::iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Archive::iterator::operator!=(ARX::Archive::iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// ARX::Archive::const_iterator                                                                  //
///////////////////////////////////////////////////////////////////////////////////////////////////

ARX::Archive::const_iterator::const_iterator(std::map<std::uint32_t, ARX::Item *>::const_iterator Iterator) :
	m_Iterator{Iterator}
{
}

ARX::Archive::const_iterator::~const_iterator()
{
}

auto ARX::Archive::const_iterator::operator++() -> ARX::Archive::const_iterator &
{
	++m_Iterator;

	return *this;
}

auto ARX::Archive::const_iterator::operator*() const -> ARX::Item const *
{
	return m_Iterator->second;
}

auto ARX::Archive::const_iterator::operator->() const -> ARX::Item const *
{
	return m_Iterator->second;
}

auto ARX::Archive::const_iterator::operator==(ARX::Archive::const_iterator const & Other) const -> bool
{
	return m_Iterator == Other.m_Iterator;
}

auto ARX::Archive::const_iterator::operator!=(ARX::Archive::const_iterator const & Other) const -> bool
{
	return m_Iterator != Other.m_Iterator;
}
