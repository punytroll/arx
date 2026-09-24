/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__SOURCE__ARCHIVEFILE_H
#define ARX__SOURCE__ARCHIVEFILE_H

#include <cstdint>

#include <arx/common.h>

#include "io.h"

/**
 * @file archive_file.h
 * 
 * An internal header file with structs and function definitions for reading ARX archive files.
 **/

namespace ARX
{
#pragma pack(1)
	/**
	 * @brief The representation of an archive's header in a file.
	 *
	 * This struct is the internal representation of an archive's header as it occures in a general ARX archive file. Reading the first sizeof(ArchiveHeader) bytes from an ARX archive file give you information about the version of the archive, the number of items contained in the archive and the ID of the root item. Thus, these information are considered absolutely neccessary for any loading process and on the other hand easy to get at the saving process.
	 **/
	class ArchiveHeader
	{
	public:
		/**
		 * @brief The major version number of the archive.
		 * @note Represents the XX.xx.xx.xx.
		 *
		 * This is the leading version number.
		 **/
		std::uint8_t MajorVersionNumber;
		
		/**
		 * @brief The minor version number of the archive.
		 * @note Represents the xx.XX.xx.xx.
		 *
		 * This is the second leading version number.
		 **/
		std::uint8_t MinorVersionNumber;
		
		/**
		 * @brief The revision number of the archive.
		 * @note Represents the xx.xx.XX.xx.
		 *
		 * This is the revision number.
		 **/
		std::uint8_t RevisionNumber;
		
		/**
		 * @brief The candidate number of the archive.
		 * @note Represents the xx.xx.xx.XX.
		 *
		 * This is the candidate number.
		 **/
		std::uint8_t CandidateNumber;
		
		/**
		 * @brief The unique identifier of the root item of the archive.
		 *
		 * This member might equal ARX::g_u4InvalidID when there is no root item specified or this archive.
		 **/
		std::uint32_t RootItemIdentifier;
		
		/**
		 * @brief The number of items stored within the archive.
		 **/
		std::uint32_t NumberOfItems;
	};
	
	/**
	 * @brief The memory representation of a single item.
	 * @note This class is located within a "pragma pack(1)" block.
	 *
	 * This class contains all the meta information associated with a specific item in an ARX archive.
	 **/
	class ItemHeader
	{
	public:
		/**
		 * @brief The identifier of the item.
		 *
		 * The identifier of an item is unique in respect to the parenting archive.
		 **/
		std::uint32_t Identifier;
		
		/**
		 * @brief The type of the item.
		 *
		 * The type of the item.
		 **/
		std::uint32_t Type;
		
		/**
		 * @brief The subtype of the item.
		 *
		 * The subtype of the item.
		 **/
		std::uint32_t SubType;
		
		/**
		 * @brief The major version number of the item.
		 * @note Represents the XX.xx.xx.xx.
		 **/
		std::uint8_t MajorVersionNumber;
		
		/**
		 * @brief The minor version number of the item.
		 * @note Represents the xx.XX.xx.xx.
		 **/
		std::uint8_t MinorVersionNumber;
		
		/**
		 * @brief The revision number of the item.
		 * @note Represents the xx.xx.XX.xx.
		 **/
		std::uint8_t RevisionNumber;
		
		/**
		 * @brief The candidate number of the item.
		 * @note Represents the xx.xx.xx.XX.
		 **/
		std::uint8_t CandidateNumber;
		
		/**
		 * @brief Information about the compression of the item.
		 *
		 * The state of compression for the item. Defines the level of compression that was used with the zlib functions or 0 to indicate no compression.
		 **/
		std::uint32_t DataCompressionType;
		
		/**
		 * @brief The length of the item's name.
		 *
		 * The length of the item's name. The name is stored right after the ItemHeader and without a final zero character.
		 **/
		std::uint32_t NameLength;
		
		/**
		 * @brief The length of the item's data when decompressed.
		 *
		 * The length of the data when it is decompressed. This member does not imply that the data actually IS decompressed. It merely gives you a 'would-be' information.
		 **/
		std::uint32_t DataDecompressedLength;
		
		/**
		 * @brief The length of the item's data when compressed.
		 *
		 * The length of the data when it is compressed. This member does not imply that the data actually IS compressed. It merely gives you a 'would-be' information. If the item's data IS compressed, this member states the length of the compressed data block. On the other hand, when the item's data is decompressed, this member is invalid. (In the current implementation this member actually states the length of the last compression, but it CAN only give you a hint about the size of the data, in case you decompressed it externaly.
		 **/
		std::uint32_t DataCompressedLength;
		
		/**
		 * @brief This component describes the length of the structure description for the item.
		 *
		 * The length of the structure buffer is needed when it will be read on loading an archive. The structure of the archive is part of its header information and therefore placed in the ItemHeader. The structure data follows immediately after the closing 0 of the item's name.
		 **/
		std::uint32_t StructureLength;
	};
#pragma pack()
    
	/**
	 * @brief The output operator for a ARX::ArchiveHeader struct.
	 * 
	 * This function takes care of the correct output of a ArchiveHeader to any kind of std::ostream.
	 **/
	inline std::ostream & operator<<(std::ostream & OStream, ARX::ArchiveHeader & ArchiveHeader)
	{
        ARX::WriteUnsignedInteger8Bit(OStream, ArchiveHeader.MajorVersionNumber);
		ARX::WriteUnsignedInteger8Bit(OStream, ArchiveHeader.MinorVersionNumber);
		ARX::WriteUnsignedInteger8Bit(OStream, ArchiveHeader.RevisionNumber);
		ARX::WriteUnsignedInteger8Bit(OStream, ArchiveHeader.CandidateNumber);
		ARX::WriteUnsignedInteger32Bit(OStream, ArchiveHeader.RootItemIdentifier);
		ARX::WriteUnsignedInteger32Bit(OStream, ArchiveHeader.NumberOfItems);
	
		return OStream;
	}
	
	/**
	 * @brief The output operator for a ARX::ItemHeader struct.
	 * 
	 * This function takes care of the correct output of a ItemHeader to any kind of std::ostream.
	 **/
	inline std::ostream & operator<<(std::ostream & OStream, ARX::ItemHeader & ItemHeader)
	{
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.Identifier);
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.Type);
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.SubType);
        ARX::WriteUnsignedInteger8Bit(OStream, ItemHeader.MajorVersionNumber);
		ARX::WriteUnsignedInteger8Bit(OStream, ItemHeader.MinorVersionNumber);
		ARX::WriteUnsignedInteger8Bit(OStream, ItemHeader.RevisionNumber);
		ARX::WriteUnsignedInteger8Bit(OStream, ItemHeader.CandidateNumber);
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.DataCompressionType);
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.NameLength);
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.DataDecompressedLength);
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.DataCompressedLength);
		ARX::WriteUnsignedInteger32Bit(OStream, ItemHeader.StructureLength);
	
		return OStream;
	}
	
	/**
	 * @brief The input operator for a ARX::ArchiveHeader struct.
	 * 
	 * This function takes care of the correct reading of a ArchiveHeader from any kind of std::istream.
	 **/
	inline std::istream & operator>>(std::istream & IStream, ARX::ArchiveHeader & ArchiveHeader)
	{
		ArchiveHeader.MajorVersionNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ArchiveHeader.MinorVersionNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ArchiveHeader.RevisionNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ArchiveHeader.CandidateNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ArchiveHeader.RootItemIdentifier = ARX::ReadUnsignedInteger32Bit(IStream);
		ArchiveHeader.NumberOfItems = ARX::ReadUnsignedInteger32Bit(IStream);
		
		return IStream;
	}
	
	/**
	 * @brief The input operator for a ARX::ItemHeader struct.
	 * 
	 * This function takes care of the correct reading of a ItemHeader from any kind of std::istream.
	 **/
	inline std::istream & operator>>(std::istream & IStream, ARX::ItemHeader & ItemHeader)
	{
		ItemHeader.Identifier = ARX::ReadUnsignedInteger32Bit(IStream);
		ItemHeader.Type = ARX::ReadUnsignedInteger32Bit(IStream);
		ItemHeader.SubType = ARX::ReadUnsignedInteger32Bit(IStream);
		ItemHeader.MajorVersionNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ItemHeader.MinorVersionNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ItemHeader.RevisionNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ItemHeader.CandidateNumber = ARX::ReadUnsignedInteger8Bit(IStream);
		ItemHeader.DataCompressionType = ARX::ReadUnsignedInteger32Bit(IStream);
		ItemHeader.NameLength = ARX::ReadUnsignedInteger32Bit(IStream);
		ItemHeader.DataDecompressedLength = ARX::ReadUnsignedInteger32Bit(IStream);
		ItemHeader.DataCompressedLength = ARX::ReadUnsignedInteger32Bit(IStream);
		ItemHeader.StructureLength = ARX::ReadUnsignedInteger32Bit(IStream);
		
		return IStream;
	}
}

#endif
