/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__ITEM_H
#define ARX__INCLUDE__ITEM_H

#include <cstdint>

#include <arx/data.h>
#include <arx/structure.h>

/**
 * @file utem.h
 * 
 * This file declares the interface of ARX::Item.
 **/

namespace ARX
{
	class Archive;
	
	/**
	 * @brief The ARX::Item is the basic class for any data storaging.
	 *
	 * The Item is a thin wrapper around ARX::Data but the inheritance ensures you can also treat it as an ARX::Data object.
	 *
	 * It adds the basics of ARX to the data storage: Names, IDs, Types, SubTypes and a connection to a parenting ARX::Archive.
	 *
	 * Additionally, at the moment it provides basic functionality for the structure, but that is about to change.
     * Structure should only be handled by external structure classes.
	 **/
	class Item : public ARX::Data
	{
	public:
		friend class ARX::Archive;
        
		static auto Create() -> ARX::Item *;
		static auto Create(std::uint32_t Identifier) -> ARX::Item *;
		static auto Create(ARX::Archive & Archive) -> ARX::Item *;
		static auto Create(ARX::Archive & Archive, std::uint32_t Identifier) -> ARX::Item *;
		static auto Create(ARX::Archive * Archive) -> ARX::Item *;
		static auto Create(ARX::Archive * Archive, std::uint32_t Identifier) -> ARX::Item *;
		static auto Delete(ARX::Item * Item) -> void;
		
		/**
		 * @brief Deleted copy constructor, as items cannot be copied.
		 **/
		Item(ARX::Item const & Item) = delete;
		
		/**
		 * @brief Deleted copy assignment operator, as items cannot be assigned to.
         **/
		auto operator=(ARX::Item const & Item) -> ARX::Item & = delete;
        
		/**
		 * @brief This function returns the identifier of @em this ARX::Item.
		 * 
		 * Returns the identifier of @em this item.
         * Remember that it is unique inside the associated archive.
		 **/
		auto GetIdentifier() const -> std::uint32_t;
		
		/**
		 * @brief Sets @em this item's unique identifier inside the library.
		 * @param Identifier The new identifier of this item.
		 * 
		 * This function is trivial for items that don't belong to any library.
         * The unique identifier has no meaning there and will just get assigned.
		 * 
		 * If @em this ARX::Item is inside an ARX:Archive the function is responsible for checking that the new identifier is not used yet.
		 **/
		auto SetIdentifier(std::uint32_t Identifier) -> void;
		
		/**
		 * @brief Returns a const reference to the items name.
		 *
		 * This function is called for const items and returns a const reference to the item's name.
		 **/
		auto GetName() const -> std::string const &;
		
		/**
		 * @brief Set the item's name.
		 * @param Name The new name of the item.
		 *
		 * This function lets you change the name of an item to any value representable in a std::string.
		 **/
		auto SetName(std::string const & Name) -> void;
		
		/**
		 * @brief Returns the type identifier of @em this item.
		 * 
		 * The type identifier has no sematic meaning to ARX or libarxx.
         * It only serves the purpose of getting a meaning by the application.
		 * The default value for the type is 0xFFFFFFFF.
		 **/
		auto GetType() const -> std::uint32_t;
		
		/**
		 * @brief Returns the sub-type identifier of @em this item.
		 * 
		 * As the type the sub-type identifier has no sematic meaning to ARX or libarxx.
         * It is meant to be placed into meaning by the application.
         * The default value for the sub type is 0xFFFFFFFF.
		 **/
		auto GetSubType() const -> std::uint32_t;
		
		/**
		 * @brief Returns the major version number of @em this item.
		 **/
		auto GetMajorVersionNumber() const -> std::uint8_t;
		
		/**
		 * @brief Returns the minor version number of @em this item.
		 **/
		auto GetMinorVersionNumber() const -> std::uint8_t;
		
		/**
		 * @brief Returns the revision number of @em this item.
		 **/
		auto GetRevisionNumber() const -> std::uint8_t;
		
		/**
		 * @brief Returns the candidate number of @em this item.
		 **/
		auto GetCandidateNumber() const -> std::uint8_t;
		
		/**
		 * @brief Enables you to set the type identifier of @em this item.
		 * @param Type The type identifier you whish to set for @em this item.
		 * 
		 * The type identifier has no sematic meaning to ARX or libarxx.
         * It only serves the purpose of getting a meaning by the application.
		 **/
		auto SetType(std::uint32_t Type) -> void;
		
		/**
		 * @brief Enables you to set the sub-type identifier of @em this item.
		 * @param SubType The sub-type identifier you whish to set for @em this item.
		 * 
		 * As the type the sub-type identifier has no sematic meaning to ARX or libarxx.
         * It is meant to be placed into meaning by the application.
		 **/
		auto SetSubType(std::uint32_t SubType) -> void;
		
		/**
		 * @brief Sets the version number of this @em item.
		 * @param MajorVersionNumber The new major version number of the item.
		 * @param MinorVersionNumber The new minor version number of the item.
		 * @param RevisionNumber The new revision number of the item.
		 * @param CandidateNumber The new candidate number of the item.
		 *
		 * Sets the version numbers of the item. Note that this information is purely related to the content and in no way related to the components or structure of the ARX::Item.
		 **/
		auto SetVersionNumbers(std::uint8_t MajorVersionNumber, std::uint8_t MinorVersionNumber, std::uint8_t RevisionNumber, std::uint8_t CandidateNumber) -> void;
		
		/**
		 * @brief Returns a pointer to the registered Archive.
		 *
		 * The function will return `nullptr` if this item belongs to no archive.
		 **/
		auto GetArchive() const -> ARX::Archive *;
		
		/**
		 * @brief Returns the Structure component associated with this Item.
		 **/
		auto GetStructure() -> ARX::Structure &;
		
		/**
		 * @brief Returns the const Structure component associated with this Item.
		 **/
		auto GetStructure() const -> ARX::Structure const &;
        
	protected:
		auto m_Fetch(std::uint32_t Offset, std::uint32_t Length) -> bool override;
        
	private:
		/**
		 * @brief The default constructor is private.
		 * @note Use ARX::Item::Create() to create new items.
		 **/
		Item();
		
		/**
		 * @brief The destructor is private.
		 * 
		 * This destructor is virtual so that you can savely derieve from the ARX::Item.
		 **/
		virtual ~Item();
		
		/**
		 * @brief The item's identifier to identify it inside a library.
		 * 
		 * The item's identifier will identify the item in a library.
         * The identifier allows access to the item in O(log(n)).
		 * Depending on the constructor this field is either initialized with 0xFFFFFFFF or with the identifier passed to the constructor.
		 **/
		std::uint32_t m_Identifier{g_InvalidItemIdentifier};
		
		/**
		 * @brief The item's name in string form.
		 * 
		 * The name is in no way meant to identify the item.
         * It's just used to have a short description of the item (although no length restriction is given).
		 * Despite this, the name can be used to build paths over the items of the library.
		 * 
		 * Paths look like this:
		 * @code
		 * /
		 * /some
		 * /some/path
		 * /[child]some/[child]path
		 * /[child]some/path/[parent]some/path
		 * /[child]some/[other]path
		 * @endcode
		 * 
		 * After creating an Item this value is intentionally "".
		 **/
		std::string m_Name;
		
		/**
		 * @brief A type identifier for @em this item.
		 * 
		 * The type of an item has no semantic meaning inside ARX or libarxx.
         * It exists only to be used by applications to identify the type of the data of @em this item.
         * Think of it as a replacement for extensions.
         * 
		 * After creating an Item this value is intentionally 0xFFFFFFFF.
		 **/
		std::uint32_t m_Type{g_InvalidItemIdentifier};
		
		/**
		 * @brief A subtype identifier for @em this item.
		 * 
		 * Like the type the subtype has no semantic meaning  inside ARX or libarxx.
         * It exists only to be used by the application to identify subtypes of types.
         * This is useful for example to distinguish between different versions of data format that belong to the same type.
         * It can also be used to highlight semantic meaning of a general data format on filesystem layer, like identifying all kinds of log files with a certain type and distinguishing between error, info, debug and critical with different subtypes.
         * 
		 * After creating an Item this value is intentionally 0xFFFFFFFF.
		 **/
		std::uint32_t m_SubType{g_InvalidItemIdentifier};
		
		/**
		 * @brief The major version number of the item.
		 **/
		std::uint8_t m_MajorVersionNumber{0};
		
		/**
		 * @brief The minor version number of the item.
		 **/
		std::uint8_t m_MinorVersionNumber{0};
		
		/**
		 * @brief The revision number of the item.
		 **/
		std::uint8_t m_RevisionNumber{0};
		
		/**
		 * @brief The candidate number of the item.
		 **/
		std::uint8_t m_CandidateNumber;
		
		/**
		 * @brief A pointer to the Archive.
		 * 
		 * Depending on the constructor this field is either initialized with `nullptr` or with the address of the archive passed to the constructor.
		 * It is changed in conjuction with the @a ARX::Archive::RegisterItem() and @a ARX::Archive::UnregisterItem() functions.
		 **/
		ARX::Archive * m_Archive{nullptr};
		
		/**
		 * @brief The structural information of an item.
		 **/
		ARX::Structure m_Structure;
	};
}

#endif
