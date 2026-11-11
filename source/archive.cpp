/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <utility>

#include <arx/archive.h>
#include <arx/buffer_writer.h>
#include <arx/item.h>
#include <arx/reference.h>

#include "io.h"
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
    try
    {
        Close();
    }
    catch(...)
    {
        // Intentionally ignored: the exception must not leave the constructor
    }
}

auto ARX::Archive::Load(std::filesystem::path const & FilePath) -> bool
{
    Close();
    m_IStream.open(FilePath);
    if(m_IStream.fail())
    {
        return false;
    }
    
    auto MajorVersionNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
    
    if(MajorVersionNumberOrError.has_value() == false)
    {
        return false;
    }
    
    auto MinorVersionNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
    
    if(MinorVersionNumberOrError.has_value() == false)
    {
        return false;
    }
    
    auto RevisionNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
    
    if(RevisionNumberOrError.has_value() == false)
    {
        return false;
    }
    
    auto CandidateNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
    
    if(CandidateNumberOrError.has_value() == false)
    {
        return false;
    }
    
    auto RootItemIdentifierOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
    
    if(RootItemIdentifierOrError.has_value() == false)
    {
        return false;
    }
    
    auto NumberOfItemsOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
    
    if(NumberOfItemsOrError.has_value() == false)
    {
        return false;
    }
    
    if((MajorVersionNumberOrError.value() == 2) && (MinorVersionNumberOrError.value() == 1) && (RevisionNumberOrError.value() == 0) && (CandidateNumberOrError.value() == 0))
    {
        if(m_Read_2_1_0_0(NumberOfItemsOrError.value()) == false)
        {
            return false;
        }
        else
        {
            m_RootItem = GetItem(RootItemIdentifierOrError.value());
        }
    }
    else
    {
        std::println(std::cerr, "Unknown ARX archive format: version = {}.{}.{}.{}.", static_cast<std::uint32_t>(MajorVersionNumberOrError.value()), static_cast<std::uint32_t>(MinorVersionNumberOrError.value()), static_cast<std::uint32_t>(RevisionNumberOrError.value()), static_cast<std::uint32_t>(CandidateNumberOrError.value()));
        
        return false;
    }
    
    return true;
}

auto ARX::Archive::Close() -> void
{
    m_IStream.close();
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

auto ARX::Archive::m_Read_2_1_0_0(std::uint32_t ItemCount) -> bool
{
    for(auto ItemIndex = std::uint32_t{0}; ItemIndex < ItemCount; ++ItemIndex)
    {
        auto IdentifierOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
        
        if(IdentifierOrError.has_value() == false)
        {
            return false;
        }
        
        auto Item = ARX::Item::Create(this, IdentifierOrError.value());
        
        if(auto TypeOrError = ARX::ReadUnsignedInteger32Bit(m_IStream); TypeOrError.has_value() == true)
        {
            Item->SetType(TypeOrError.value());
        }
        else
        {
            return false;
        }
        if(auto SubTypeOrError = ARX::ReadUnsignedInteger32Bit(m_IStream); SubTypeOrError.has_value() == true)
        {
            Item->SetSubType(SubTypeOrError.value());
        }
        else
        {
            return false;
        }
        
        auto MajorVersionNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
        
        if(MajorVersionNumberOrError.has_value() == false)
        {
            return false;
        }
        
        auto MinorVersionNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
        
        if(MinorVersionNumberOrError.has_value() == false)
        {
            return false;
        }
        
        auto RevisionNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
        
        if(RevisionNumberOrError.has_value() == false)
        {
            return false;
        }
        
        auto CandidateNumberOrError = ARX::ReadUnsignedInteger8Bit(m_IStream);
        
        if(CandidateNumberOrError.has_value() == false)
        {
            return false;
        }
        Item->SetVersionNumbers(MajorVersionNumberOrError.value(), MinorVersionNumberOrError.value(), RevisionNumberOrError.value(), CandidateNumberOrError.value());
        
        auto DataCompressionTypeOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
        
        if(DataCompressionTypeOrError.has_value() == false)
        {
            return false;
        }
        
        auto NameLengthOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
        
        if(NameLengthOrError.has_value() == false)
        {
            return false;
        }
        
        auto DataDecompressedLengthOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
        
        if(DataDecompressedLengthOrError.has_value() == false)
        {
            return false;
        }
        
        auto DataCompressedLengthOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
        
        if(DataCompressedLengthOrError.has_value() == false)
        {
            return false;
        }
        
        auto StructureLengthOrError = ARX::ReadUnsignedInteger32Bit(m_IStream);
        
        if(StructureLengthOrError.has_value() == false)
        {
            return false;
        }
        
        auto Name = std::vector<char>(NameLengthOrError.value() + 1);
        
        m_IStream.read(Name.data(), NameLengthOrError.value() + 1);
        Name[NameLengthOrError.value()] = '\0';
        Item->SetName(Name.data());
        if(Item->GetStructure().m_ReadFromStream(StructureLengthOrError.value(), m_IStream).has_value() == false)
        {
            return false;
        }
        Item->SetFetchInformation(m_IStream.tellg(), static_cast<ARX::Data::Compression>(DataCompressionTypeOrError.value()), DataDecompressedLengthOrError.value(), DataCompressedLengthOrError.value());
        m_IStream.seekg((DataCompressionTypeOrError.value() == 0) ? (DataDecompressedLengthOrError.value()) : (DataCompressedLengthOrError.value()), std::ios_base::cur);
    }
    
    return true;
}

auto ARX::Archive::Fetch(std::uint32_t Offset, std::uint32_t Length, ARX::Buffer * Buffer) -> bool
{
    m_IStream.seekg(Offset, std::ios_base::beg);
    Buffer->SetLength(0);
    
    auto BufferWriter = ARX::BufferWriter{*Buffer};
    
    BufferWriter << std::make_pair(static_cast<ARX::Buffer::size_type>(Length), dynamic_cast<std::istream *>(std::addressof(m_IStream)));
    
    return true;
}

auto ARX::Archive::Save(std::filesystem::path const & FilePath, bool AutoCompress) -> void
{
    auto TemporaryFilePath = std::filesystem::temp_directory_path() / ("ARX" + std::to_string(time(0)));
    auto OStream = std::ofstream{TemporaryFilePath};
    
    if(!OStream)
    {
        throw ARX::file_error{TemporaryFilePath};
    }
    ARX::WriteUnsignedInteger8Bit(OStream, 2);
    ARX::WriteUnsignedInteger8Bit(OStream, 1);
    ARX::WriteUnsignedInteger8Bit(OStream, 0);
    ARX::WriteUnsignedInteger8Bit(OStream, 0);
    ARX::WriteUnsignedInteger32Bit(OStream, (m_RootItem != nullptr) ? (m_RootItem->GetIdentifier()) : (g_InvalidItemIdentifier));
    ARX::WriteUnsignedInteger32Bit(OStream, m_Items.size());
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
        
        ARX::Data StructureBuffer;
        
        StructureBuffer << Item->GetStructure();
        ARX::WriteUnsignedInteger32Bit(OStream, Item->GetIdentifier());
        ARX::WriteUnsignedInteger32Bit(OStream, Item->GetType());
        ARX::WriteUnsignedInteger32Bit(OStream, Item->GetSubType());
        ARX::WriteUnsignedInteger8Bit(OStream, Item->GetMajorVersionNumber());
        ARX::WriteUnsignedInteger8Bit(OStream, Item->GetMinorVersionNumber());
        ARX::WriteUnsignedInteger8Bit(OStream, Item->GetRevisionNumber());
        ARX::WriteUnsignedInteger8Bit(OStream, Item->GetCandidateNumber());
        ARX::WriteUnsignedInteger32Bit(OStream, std::to_underlying(Item->GetCompression()));
        ARX::WriteUnsignedInteger32Bit(OStream, Item->GetName().length());
        ARX::WriteUnsignedInteger32Bit(OStream, Item->GetDecompressedLength());
        ARX::WriteUnsignedInteger32Bit(OStream, Item->GetCompressedLength());
        ARX::WriteUnsignedInteger32Bit(OStream, StructureBuffer.GetLength());
        OStream << Item->GetName() << '\0' << StructureBuffer << *Item;
    }
    OStream.close();
    if(auto ChildPID = fork(); ChildPID == 0)
    {
        execlp("mv", "mv", "--force", TemporaryFilePath.string().c_str(), FilePath.c_str(), reinterpret_cast<char *>(0));
    }
    else
    {
        auto Return = 0;
        
        waitpid(ChildPID, &Return, 0);
    }
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
