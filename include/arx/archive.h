/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__ARCHIVE_H
#define ARX__INCLUDE__ARCHIVE_H

#include <map>
#include <vector>

#include <arx/common.h>

/**
 * @file archive.h
 * 
 * This file declares the ARX::Archive class and ARX::Archive::iterator and ARX::Archive::const_iterator iterator classes.
 **/

namespace ARX
{
	class Buffer;
	class Item;
	class Reference;
	class ReferenceCore;
	
	/**
	 * @brief The ARX::Archive class is a container for an arbitrary amount of ARX::Item objects.
	 *
	 * The main purpose of ARX::Archive is to handle input and output from and to ARX archives.
     * But it also is responsible for the management of unique IDs and lets you access items you only know by ID.
	 * In order to be able to manage input and output ARX::Archive provides two functions: @a ARX::Archive::Load() and @a ARX::Archive::Save().
	 **/
	class Archive
	{
	public:
		/**
		 * @brief An iterator class defining basic operations to iterate the ARX::Item objects in an archive.
		 * 
		 * This iterator class encapsulates the internal form of Item object storage in an archive.
         * By using this iterator class, which is what you have to do, you are safe from changes in implementation detail as the container used to store ARX::Item object pointers.
         * Additionally it is the prefered way to make a visit to each item in an archive.
		 **/
		class iterator
		{
		public:
			/**
			 * @brief The constructor of an ARX::Item iterator.
			 * @param Iterator An iterator from ARX::Archive's item container.
			 **/
			explicit iterator(std::map<std::uint32_t, ARX::Item *>::iterator Iterator);
			
			/**
			 * @brief The destructor of an item iterator.
			 **/
			~iterator();
			
			/**
			 * @brief Advances the iterator.
			 * 
             * It is not defined which will be the next item the iterator points to.
             * It is only assured that every item will be passed once.
			 **/
			auto operator++() -> ARX::Archive::iterator &;
			
			/**
			 * @brief This will return the ARX::Item pointer that this iterator refers to.
			 * 
			 * This is the non-const version of the operator*(), so it returns a non-const Item pointer.
			 **/
			auto operator*() -> ARX::Item *;
			
			/**
			 * @brief This will return the ARX::Item pointer that this iterator refers to.
			 * 
			 * This is the const version of the operator*(), thus the returned Item pointer is const as well.
			 **/
			auto operator*() const -> ARX::Item const *;
			
			/**
			 * @brief The ARX::Item accessor.
			 **/
			auto operator->() -> ARX::Item *;
			
			/**
			 * @brief Tests two iterators for equality.
			 * @param Other The iterator to check against.
			 **/
			auto operator==(ARX::Archive::iterator const & Other) const -> bool;
			
			/**
			 * @brief Tests two iterators for inequality.
			 * @param Other The iterator to check against.
			 **/
			auto operator!=(ARX::Archive::iterator const & Other) const -> bool;
            
		private:
			/**
			 * @brief The iterator into the archive's item container.
			 **/
			std::map<std::uint32_t, ARX::Item *>::iterator m_Iterator;
            
		};
		
		/**
		 * @brief An iterator class defining basic operations to iterate the ARX::Item objects in an ARX::Archive.
		 * 
		 * This iterator class encapsulates the internal form of Item object storage in an archive.
         * By using this iterator class, which is what you have to do, you are safe from changes in implementation detail as the container used to store Item object pointers.
         * Additionally it is the prefered way to make a visit to each Item in an archive.
		 **/
		class const_iterator
		{
		public:
			/**
			 * @brief The constructor of a const Item iterator.
			 * @param Iterator An iterator from ARX::Archive's item container.
			 **/
			explicit const_iterator(std::map<std::uint32_t, ARX::Item *>::const_iterator Iterator);
			
			/**
			 * @brief The destructor of a const Item iterator.
			 **/
			~const_iterator();
			
			/**
			 * @brief Advances the iterator.
			 * 
             * It is not defined which will be the next item the iterator points to.
             * It is only assured that every item will be passed once.
			 **/
			auto operator++() -> ARX::Archive::const_iterator &;
			
			/**
			 * @brief This will return the ARX::Item that this iterator points to.
			 * 
			 * This is the const version of the operator*(), thus the returned item pointer is const as well.
			 **/
			auto operator*() const -> ARX::Item const *;
			
			/**
			 * @brief The ARX::Item accessor.
			 **/
			auto operator->() const -> ARX::Item const *;
			
			/**
			 * @brief Tests two iterators for equality.
			 * @param Other The iterator to check against.
			 **/
			auto operator==(const ARX::Archive::const_iterator & Other) const -> bool;
			
			/**
			 * @brief Tests two iterators for inequality.
			 * @param Other The iterator to check against.
			 **/
			auto operator!=(const ARX::Archive::const_iterator & Other) const -> bool;
            
		private:
			/**
			 * @brief The iterator into the archive's item container.
			 **/
			std::map<std::uint32_t, ARX::Item *>::const_iterator m_Iterator;
            
		};
		
		typedef std::map<std::uint32_t, ARX::Item *>::size_type size_type;
		
		/**
		 * @brief The default constructor.
		 *
		 * This constructor creates and initialises an ARX::Archive.
		 **/
		Archive();
        
		/**
		 * @brief No copy semantic.
		 **/
		Archive(ARX::Archive const & Other) = delete;
		
		/**
		 * @brief The destructor for a ARX::Archive.
		 * 
		 * The destructor will call ARX::Archive::Close().
		 **/
		virtual ~Archive();
		
		/**
		 * @brief No assignment semantic.
		 **/
		auto operator=(ARX::Archive const & Other) -> ARX::Archive & = delete;
		
		/**
		 * @brief Load the content of an ARX archive into this ARX::Archive.
		 * @param FilePath The path to the archive file.
		 * @return A boolean value indicating whether the ARX archive could be loaded or not.
         *         An error can have many reasons (file existance, permissions and content) but there is no facility to report which error occured.
         *         If you feel a need for it, please notify me.
		 * 
		 * This function tries to load the ARX archive at the location indicated by @a FilePath.
         * If the archive is found and could be opened, the current content of the ARX::Archive will be closed (via @a ARX::Archive::Close()).
         * Note that if the archive proves to be no valid ARX archive the current content is not available anymore.
		 **/
		auto Load(std::string const & FilePath) -> bool;
		
		/**
		 * @brief Saves the content to an ARX archive.
		 * @param FilePath The path to the new archive file.
		 * @param AutoCompress Whether all ARX::Item objects should be auto-compressed.
		 * @throw ARX::file_error If the temporary archive in /tmp cannot be created for writing.
		 *
		 * Given the @a FilePath, the content of an ARX::Archive instance will be saved to the location it indicates. The target will be overwritten if it exists but only after it has been fully assembled in a temporary archive. The temorary archive will be stored under "/tmp/" with the name "ARX" followed by the timestamp. After the saving process to that temporary file it will be moved to the target location, using the command `mv --force PathToTemporaryFile @a FilePath`
		 */
		auto Save(std::string const & FilePath, bool AutoCompress = false) -> void;
		
		/**
		 * @brief Closes the Archive.
		 * 
		 * Closing an archive means purging it of all item objects.
		 **/
		auto Close() -> void;
		
		/**
		 * @brief Registers an Item to be a part of an Archive.
		 * @param Item A reference to the Item that is to be registerd.
		 * @throw std::invalid_argument If @a Item is already registered at another Archive.
		 * 
		 * First of all this function will check whether the item is already assigned to an Archive and will throw an exception of type std::invalid_argument.
		 * If the Item is not yet registered at another Archive, this function will set ARX::Item::m_Archive to point to this Archive.
		 * 
		 * If the Item is not yet associated to any ARX::Archive this function will decide on one of the following behavior, depending on whether the unique ID of @a Item is equal to g_InvalidItemIdentifier or not.
		 * - If the unique ID is equal to g_InvalidItemIdentifier this function will search for a unique item identifier not yet used inside this Archive.
		 * - If the unique ID is any other value, this function will allow the registration if that ID is not yet used by another item in this Archive.
         *   Not alowing a registration means to throw an exception of the type ARX::id_not_unique.
		 **/
		auto Register(ARX::Item * Item) -> void;
		
		/**
		 * @brief Unregisters an Item.
		 * @param Item A reference to the Item that is to be unregisterd.
		 * @throw std::invalid_argument If @a Item is not registered in the Archive.
		 * 
		 * This function will remove @a Item from an Archive by erasing the appropriate iterator from the Item object container owned by the Archive.
		 * If @a Item is not correctly registered in the Archive an std::invalid_argument exception is thrown.
		 * 
		 * @note This function will change @a Item! Note that the item's @a m_Archive field will be set to `nullptr` if no errors occure.
		 **/
		auto Unregister(ARX::Item * Item) -> void;
		
		/**
		 * @brief Sets a root Item.
		 * @param Item A pointer to an ARX::Item that is to be set as the archive's root item or nullptr.
		 * @throw std::invalid_argument If the Item that @a pItem points to is not registered in the Archive.
		 * 
		 * This function will modify the @a m_RootItem member of the Archive setting it to @a Item.
		 * 
		 * You may pass @a Item `nullptr` to unset the root Item of the Archive.
		 **/
		auto SetRootItem(ARX::Item * Item) -> void;
		
		/**
		 * @brief This function finds the item with a given item identifier.
		 * @param ItemIdentifier The identifier of the item which is to be found in the archive.
		 * 
		 * If the identifier is unknown in the archive, the function will return `nullptr`.
		 * 
		 * @note This function guarantees to be of complexity in O(log(n)) where n is the number of items in the archive.
		 **/
		auto GetItem(std::uint32_t ItemIdentifier) -> ARX::Item *;
		
		/**
		 * @brief This function finds the item with a given item identifier.
		 * @param ItemIdentifier The identifier of the item which is to be found in the archive.
		 * 
		 * If the identifier is unknown in the archive this function will return `nullptr`.
		 * 
		 * @note This function guarantees to be of complexity in O(log(n)) where n is the number of items in the archive.
		 **/
		auto GetItem(std::uint32_t ItemIdentifier) const -> ARX::Item const *;
		
		/**
		 * @brief Retrieves an item from the archive that is identified by a given path.
		 * @param Path A string with the path information.
		 * @return A pointer to an item that is identified by the path expression.
		 * 
		 * Based on the name attribute of items and the structural information, this function walks through the @a path trying to find the appropriate item, or returns `nullptr`.
		 * 
		 * @note Since item names do not need to be unique inside a relation, this function only finds the first item on the path.
		 * 
		 * The default relation to be searched is "child". However, the @a Path may contain information concerning the name of the relation that is to be searched for the appropriate item:
		 * @code /sub/one::name/down::another @endcode
		 * 
		 * In this path the first item name is "sub" and an item with that name is searched inside the "child" relation of the root item.
         * The second item name is "name" which needs to be located inside the "one" relation of the item "/sub".
         * The third item name is "another" which needs to be located inside the "down" relation.
		 **/
		auto GetItem(std::string Path) -> ARX::Item *;
		
		/**
		 * @brief Retrieves a const item from the archive that is identified by a given path.
		 * @param Path A string with the path information.
		 * @return A pointer to an item that is identified by the path expression.
		 * 
		 * Based on the name attribute of items and the structural information this function walks through the @a Path trying to find the appropriate item, or returns `nullptr`.
		 * 
		 * @note Since item names do not need to be unique inside a relation, this function only finds the first item at the path.
		 * 
		 * The default relation to be searched is "child". However, the @a Path may contain information concerning the name of the relation that is to be searched for the appropriate item:
		 * @code /sub/one::name/down::another @endcode
		 * 
		 * In this path the first item name is "sub" and an item with that name is searched inside the "child" relation of the root item.
         * The second item name is "name" which needs to be located inside the "one" relation of the item "/sub".
         * The third item name is "another" which needs to be located inside the "down" relation.
		 **/
		auto GetItem(std::string Path) const -> ARX::Item const *;
		
		/**
		 * @brief Provides access to the root item.
		 * 
		 * This function returns the m_RootItem member of the archive, so if the archive has no root item set, the function will return `nullptr`.
		 **/
		auto GetRootItem() -> ARX::Item *;
		
		/**
		 * @brief Provides const access to the root item.
		 * 
		 * This function returns the m_RootItem member of the archive, so if the archive has no root item set, the function will return `nullptr`.
		 **/
		auto GetRootItem() const -> ARX::Item const *;
		
		/**
		 * @brief Returns a ARX::Reference for a given item identifier.
		 * @param ItemIdentifier The item identifier which you will get an ARX::Reference for.
		 * 
		 * If a ARX::Item object with the given item identifier is registered in the Archive the Reference will be resolved.
         * If no such Item is registered the Reference will be unresolved, but will be linked to the Archive, so that, once an Item with that unique ID is registered at the Archive, the Reference will be resolved.
		 **/
		auto GetReference(std::uint32_t ItemIdentifier) -> ARX::Reference;
		
		/**
		 * @brief Returns a ARX::Archive::iterator to the first item.
		 * 
		 * This function is provided so you can iterate through all the items registered in an Archive.
		 **/
		auto begin() -> ARX::Archive::iterator;
		
		/**
		 * @brief Returns a ARX::Archive::iterator behind the last item.
		 * 
		 * This function is provided so you can iterate through all the items registered in an Archive.
		 **/
		auto end() -> ARX::Archive::iterator;
		
		/**
		 * @brief Returns a ARX::Archive::const_iterator to the first item.
		 * 
		 * This function is provided so you can iterate through all the items registered in a const Archive.
		 **/
		auto begin() const -> ARX::Archive::const_iterator;
		
		/**
		 * @brief Returns a ARX::Archive::const_iterator behind the last item.
		 * 
		 * This function is provided so you can iterate through all the items registered in a const Archive.
		 **/
		auto end() const -> ARX::Archive::const_iterator;
		
		/**
		 * @brief Returns the number of Item objects.
		 * 
		 * This function returns the number of ARX::Item objects registered in the Archive. It is designed to be STL compatible.
		 **/
		auto size() const -> ARX::Archive::size_type;
		
		/**
		 * @brief Returns the number of resolved and unresolved references in the Archive.
		 **/
		auto GetNumberOfReferences() const -> std::uint32_t;
		
		/**
		 * @brief This function releases a Reference inside the Archive.
		 * 
		 * This is an internal function, but as the API user will never lay hands on a ARX::ReferenceCore object it is still public to avoid friend declarations.
		 * This function is called whenever the reference count of an unresolved ARX::ReferenceCore object reaches `1`, because in that case the associated Archive object is the only reference holder of the ReferenceCore object.
         * Since unresolved References serve no purpose other than to be resolved magically there is no sense in keeping it only inside the archive.
		 * This function will retrieve the associated Reference object and will remove it from the m_References map thus releasing the ReferenceCore object which then is deleted.
		 **/
		auto ReleaseReferenceCore(ARX::ReferenceCore * ReferenceCore) -> void;
		
		/**
		 * @brief Fetch data from the archive file into the buffer.
		 * @param Offset The offset inside the archive file.
		 * @param Length The amount of data to read from the archive file.
		 * @param Buffer The buffer to fill with the data.
		 **/
		auto Fetch(std::uint32_t Offset, std::uint32_t Length, ARX::Buffer * Buffer) -> bool;
        
	private:
		/**
		 * @brief The reader function that retrieves Item header information from a channel.
		 **/
		auto m_Read_2_1_0_0(std::uint32_t ItemCount) -> void;
		
        /**
         * @brief Internal item container.
         * 
         * Maps from the item's UID to the item.
         **/
		std::map<std::uint32_t, ARX::Item *> m_Items;
        
        /**
         * @brief Map of item references in the archive.
         * 
         * Item references are created for all UIDs that are referenced from the relations of items in this archive.
         * Every occurence of any UID referenced in any relation of any item in this archive is added here, and all occurences share the same "core", which makes them all resolve at the same time, if they are reloved at any point in time.
         **/
		std::map<std::uint32_t, ARX::Reference> m_References;
        
        /**
         * @brief A pointer to the ARX::Item that is at the root of this archive, if any.
         **/
		ARX::Item * m_RootItem{nullptr};
		
		/**
		 * @brief The archive file stream.
		 **/
		std::istream * m_IStream{nullptr};
        
	};
}

#endif
