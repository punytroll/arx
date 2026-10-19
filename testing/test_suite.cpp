/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef TEST_DATA_DIRECTORY_PATH
#error TEST_DATA_DIRECTORY_PATH must be set!
#endif

#include <cassert>
#include <filesystem>

#include <arx/archive.h>
#include <arx/item.h>

auto OpenArchive(std::filesystem::path const & ArchivePath) -> std::unique_ptr<ARX::Archive>
{
    auto Archive = std::make_unique<ARX::Archive>();
    
    Archive->Load(ArchivePath.native());
    
    return Archive;
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char * argv[]) -> int
{
    auto TestDataDirectoryPath = std::filesystem::path{TEST_DATA_DIRECTORY_PATH};
    
    {
        auto Archive = OpenArchive(TestDataDirectoryPath / "2.1.0.0" / "001_empty.arx");
        
        assert(Archive->size() == 0);
        assert(Archive->GetNumberOfReferences() == 0);
        assert(Archive->GetRootItem() == nullptr);
    }
    {
        auto Archive = OpenArchive(TestDataDirectoryPath / "2.1.0.0" / "002_root_item.arx");
        
        assert(Archive->size() == 1);
        assert(Archive->GetNumberOfReferences() == 1);
        
        auto RootItem = Archive->GetRootItem();
        
        assert(RootItem != nullptr);
        assert(RootItem->GetName() == "test root");
        assert(RootItem->GetType() == 0x1234);
        assert(RootItem->GetSubType() == 0x8888);
        assert(RootItem->GetMajorVersionNumber() == 1);
        assert(RootItem->GetMinorVersionNumber() == 2);
        assert(RootItem->GetRevisionNumber() == 3);
        assert(RootItem->GetCandidateNumber() == 4);
        assert(RootItem->IsCompressed() == false);
        assert(RootItem->GetDecompressedLength() == 20);
    }
    
    return 0;
}
