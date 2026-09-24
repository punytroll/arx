/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__SOURCE__REFERENCE_CORE_H
#define ARX__SOURCE__REFERENCE_CORE_H

#include <arx/common.h>

namespace ARX
{
	class Item;
	class Archive;
	
	/**
	 * @brief The core of an ARX::Reference, which contains a reference counter, the item identifier, the item pointer, if it has been resolved and a library pointer.
	 **/
	class ReferenceCore
	{
	public:
		/**
		 * @brief Returns a new ReferenceCore instance created from a ARX::Item.
		 * @param Item The ARX::Item that the new reference should refer to.
		 * 
		 * Of course, the new reference is resolved.
		 **/
		static auto Create(ARX::Item & Item) -> ARX::ReferenceCore *;
		
		/**
		 * @brief Returns a new ReferenceCore instance created from an item identifier and an archive.
		 * @param ItemIdentifier The unique ID that the new reference shoud refer to.
		 * @param Archive Since unique IDs are not strictly bound to but only sensible in Archives, this lets you pass the appropriate Archive.
		 * 
		 * The new reference of course is unresolved.
		 **/
		static auto Create(std::uint32_t ItemIdentifier, ARX::Archive * Archive = nullptr) -> ARX::ReferenceCore *;
		
		/**
		 * @brief Increases the reference counter and conveniently returns the parameter.
		 * @param ReferenceCore The ReferenceCore object that you want to reference.
		 * 
		 * Since the new reference is equal to the old reference its resolved status is equal as well.
		 **/
		static auto Create(ARX::ReferenceCore * ReferenceCore) -> ARX::ReferenceCore *;
		
		/**
		 * @brief Decreases the reference counter and possibly deletes the reference.
		 * @param ReferenceCore The ReferenceCore object that you want to release.
		 * @return A boolean value indicating whether the reference was deleted or not.
		 * 
		 * If decreasing the reference counter of @a ReferenceCore make it equal to zero, the ReferenceCore is deleted.
		 **/
		static auto Release(ARX::ReferenceCore * ReferenceCore) -> bool;
		
		/**
		 * @brief No copy semantic for ReferenceCore objects.
		 **/
		ReferenceCore(ARX::ReferenceCore const & Reference) = delete;
		
		/**
		 * @brief No assigment semantic for ReferenceCore objects.
		 **/
		auto operator=(ARX::ReferenceCore const & Reference) -> ARX::ReferenceCore & = delete;
		
		/**
		 * @brief Returns the unique ID of the reference.
		 * 
		 * It does not matter if the reference is resolved or unresolved, the unique ID is certain to be returned.
		 **/
		auto GetItemIdentifier() const -> std::uint32_t;
		
		/**
		 * @brief Returns the ARX::Item pointer of the reference.
		 * 
		 * If the reference is resolved this correctly returns the ARX::Item's pointer.
         * In case of an unresolved reference the returned pointer is `nullptr`.
		 **/
		auto GetItem() -> ARX::Item *;
		
		/**
		 * @brief Returns the ARX::Item pointer of the reference.
		 * 
		 * If the reference is resolved this correctly returns the ARX::Item's pointer.
         * In case of an unresolved reference the returned pointer is `nullptr`.
		 **/
		auto GetItem() const -> ARX::Item const *;
		
		/**
		 * @brief Returns m_ReferenceCounter, the number of ARX::Reference objects that hold @em this ReferenceCore object.
		 **/
		auto GetReferenceCount() const -> std::uint32_t;
		
		/**
		 * @brief Resolves an unresolved reference with a given item.
		 * @param Item The ARX::Item that will be used to resolve the reference.
		 * 
		 * This functions checks for two things:
		 * - The reference must not be resolved yet.
		 * - The @a Item must have the same ID as the unresolved reference.
		 * 
		 * Not meeting one of these requirements will throw an std::runtime_error exception.
		 **/
		auto Resolve(ARX::Item & Item) -> void;
		
		/**
		 * @brief Unresolves the resolved reference by setting its ARX::Item pointer to `nullptr`.
		 **/
		auto Unresolve() -> void;
		
		/**
		 * @brief This function sets m_Archive to `nullptr`.
		 **/
		auto DecoupleFromArchive() -> void;
        
	private:
		/**
		 * @brief The constructor of a ReferenceImplementation object.
		 * 
		 * Instances of this class are only created via calls to the static Create() functions.
		 **/
		ReferenceCore() = default;
		
		/**
		 * @brief The destructor of a ReferenceImplementation object.
		 * 
		 * Instances of this class may only be destroyed via calls to the static Release() function.
		 **/
		~ReferenceCore();
		
		std::uint32_t m_ItemIdentifier{0};
		ARX::Item * m_Item{nullptr};
		std::uint32_t m_ReferenceCount{1};
		ARX::Archive * m_Archive{nullptr};
	};
}

#endif
