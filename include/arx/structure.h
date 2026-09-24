/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__STRUCTURE_H
#define ARX__INCLUDE__STRUCTURE_H

#include <list>
#include <map>
#include <vector>

#include <arx/common.h>
#include <arx/reference.h>

/**
 * @file structure.h
 * 
 * This file declares the Structure class, the Structure::Relation class and iterator classes for both.
 **/

namespace ARX
{
	class Archive;
	class Buffer;
	class Item;
	class Structure;
	
	/**
	 * @brief Convenience function that streams the structural information into a ARX::Buffer.
	 **/
	auto operator<<(ARX::Buffer & Buffer, ARX::Structure const & Structure) -> ARX::Buffer &;
	
	/**
	 * @brief The structure of a ARX archive is saved inside these objects.
	 * 
	 * This Structure class provides a general but standarized way to structure the items in an archive.
	 **/
	class Structure
	{
	public:
		friend class ARX::Archive;
        
		/**
		 * @brief The object that holds references to other items.
		 * 
		 * A Relation is a container for ARX::Reference objects, which are identified by the unique ID of the referenced ARX::Item.
         * A Relation is a multiset so that one ARX::Item may be referenced multiple times.
		 **/
		class Relation
		{
		public:
			friend class ARX::Structure;
            
			typedef std::multimap<std::uint32_t, ARX::Reference>::size_type size_type;
			
			class const_iterator;
			
			/**
			 * @brief An iterator class defining basic operations to iterate the Reference objects in a Relation.
			 * 
			 * This iterator class encapsules the internal form of Reference storage in a Relation.
             * By using this iterator class, which is what you have to do, you are safe from changes in implementation detail as the container used to store Reference objects.
             * Additionally it is the prefered way to make a visit to each Reference in a Relation.
			 **/
			class iterator
			{
			public:
				friend class ARX::Structure::Relation::const_iterator;
                
				/**
				 * @brief The constructor of a Reference iterator.
				 * @param Iterator An iterator from the Relation's aggregated Reference container.
				 **/
				explicit iterator(std::multimap<std::uint32_t, ARX::Reference>::iterator Iterator);
				
				/**
				 * @brief Advances the iterator.
				 *
				 * Advances the Reference iterator.
                 * It is not defined which will be the next Reference the iterator points to.
                 * It is only assured that every Reference in the Relation will be passed once.
				 **/
				auto operator++() -> ARX::Structure::Relation::iterator &;
				
				/**
				 * @brief This will return the Reference that this iterator points to.
				 * 
				 * This is the non-const version of the operator*(), so it returns a non-const Reference.
				 **/
				auto operator*() -> ARX::Reference &;
				
				/**
				 * @brief This will return the Reference that this iterator points to.
				 * 
				 * This is the const version of the operator*(), thus the returned Reference is const as well.
				 **/
				auto operator*() const -> ARX::Reference const &;
				
				/**
				 * @brief The Reference accessor.
				 **/
				auto operator->() -> ARX::Reference *;
				
				/**
				 * @brief Tests two iterators for equality.
				 * @param Other The iterator to check against.
				 **/
				auto operator==(ARX::Structure::Relation::iterator const & Other) const -> bool;
				
				/**
				 * @brief Tests two iterators for inequality.
				 * @param Other The iterator to check against.
				 **/
				auto operator!=(ARX::Structure::Relation::iterator const & Other) const -> bool;
				
				/**
				 * @brief Tests two iterators for equality.
				 * @param Other The const_iterator to check against.
				 **/
				auto operator==(ARX::Structure::Relation::const_iterator const & Other) const -> bool;
				
				/**
				 * @brief Tests two iterators for inequality.
				 * @param Other The const_iterator to check against.
				 **/
				auto operator!=(ARX::Structure::Relation::const_iterator const & Other) const -> bool;
                
			private:
				/**
				 * @brief The iterator into the Relation's reference container.
				 **/
				std::multimap<std::uint32_t, ARX::Reference>::iterator m_Iterator;
                
			};
			
			/**
			 * @brief An iterator class defining basic operations to iterate the Reference objects in a Relation.
			 *
			 * This iterator class encapsules the internal form of Reference storage in a Relation.
             * By using this iterator class, which is what you have to do, you are safe from changes in implementation detail as the container used to store Reference objects.
             * Additionally it is the prefered way to make a visit to each Reference in a Relation.
			 **/
			class const_iterator
			{
			public:
				friend class ARX::Structure::Relation::iterator;
                
				/**
				 * @brief The constructor of a const Reference iterator.
				 * @param Iterator An iterator from the Relation's aggregated Reference container.
				 **/
				explicit const_iterator(std::multimap<std::uint32_t, ARX::Reference>::const_iterator Iterator);
				
				/**
				 * @brief The constructor of a const Reference iterator from a non-const iterator.
				 * @param Iterator An iterator of a Reference.
				 *
				 * This constructor creates a const Reference iterator from a non-const Reference iterator.
				 **/
				const_iterator(ARX::Structure::Relation::iterator Iterator);
				
				/**
				 * @brief Advances the iterator.
				 *
				 * Advances the Reference iterator.
                 * It is not defined which will be the next Reference the iterator points to.
                 * It is only assured that every Reference in the Relation will be passed once.
				 **/
				auto operator++() -> ARX::Structure::Relation::const_iterator &;
				
				/**
				 * @brief This will return the ARX::Reference that this iterator points to.
				 * 
				 * This is the const version of the operator*(), thus the returned Reference is const as well.
				 **/
				auto operator*() const -> ARX::Reference const &;
				
				/**
				 * @brief The Reference accessor.
				 **/
				auto operator->() -> ARX::Reference const *;
				
				/**
				 * @brief Tests two iterators for equality.
				 * @param Other The iterator to check against.
				 **/
				auto operator==(ARX::Structure::Relation::const_iterator const & Other) const -> bool;
				
				/**
				 * @brief Tests two iterators for inequality.
				 * @param Other The iterator to check against.
				 **/
				auto operator!=(ARX::Structure::Relation::const_iterator const & Other) const -> bool;
                
			private:
				/**
				 * @brief The iterator into the Relation's reference container.
				 **/
				std::multimap<std::uint32_t, ARX::Reference>::const_iterator m_Iterator;
                
			};
			
			/**
			 * @brief Adds an item identifer reference to the relation.
			 * @param ItemIdentifier The item identifier that is to be added to the relation.
			 * 
			 * This function adds a reference to an item identifier to the relation.
			 **/
			auto Add(std::uint32_t ItemIdentifier) -> void;
			
			/**
			 * @brief Removes a reference with a certain item identifier from the relation.
			 * @param ItemIdentifier The item identifier that is to be deleted from the relation.
			 * 
			 * This function removes a reference with a certain item identifier from the relation.
			 **/
			auto Delete(std::uint32_t ItemIdentifier) -> bool;
			
			/**
			 * @brief Gets a list of pointers to the items with the specified name.
			 * @param Name The name of the items which are to be retrieved.
			 * @return All items in the relation with a the name @a Name.
			 * @note Should the relation contain an item with the name @a Name multiple times it will be returned multiple times.
			 * @note All unresolved item references will be skipped.
			 * 
			 * This function can be used to retrieve a pointers to items that are linked to in this relation.
			 * Relations contain items without regard of their names.
             * Therefore multiple items inside a relation may have the same name and all these items are returned.
			 **/
			auto GetItems(std::string const & Name) -> std::list<ARX::Item *>;
			
			/**
			 * @brief Returns the number of references in the relation.
			 **/
			auto size() const -> size_type;
			
			/**
			 * @brief Returns the first reference iterator of the relation.
			 **/
			auto begin() -> ARX::Structure::Relation::iterator;
			
			/**
			 * @brief Returns the end reference iterator of the relation.
			 **/
			auto end() -> ARX::Structure::Relation::iterator;
			
			/**
			 * @brief Returns the first reference iterator of the relation.
			 **/
			auto begin() const -> ARX::Structure::Relation::const_iterator;
			
			/**
			 * @brief Returns the end reference iterator of the relation.
			 **/
			auto end() const -> ARX::Structure::Relation::const_iterator;
			
			/**
			 * @brief Returns the name of the relation.
			 **/
			auto GetName() const -> std::string const &;
            
		private:
			/**
			 * @brief A constructor that links a relation to a specific ARX::Structure.
			 * @param Structure The structure object that this relation is linked to.
			 * @param Name The unique name of the relation inside the related Structure.
			 **/
			Relation(ARX::Structure & Structure, std::string const & Name);
			
			/**
			 * @brief The related Structure object.
			 * 
			 * This is set by the constructor and cannot be changed.
             * Every Relation object is strictly bound to exactly one Structure at creation time.
			 **/
			ARX::Structure & m_Structure;
			
			/**
			 * @brief The Item references stored in the Relation.
			 **/
			std::multimap<std::uint32_t, ARX::Reference> m_References;
			
			/**
			 * @brief The name of the relation.
			 **/
			std::string const m_Name;
            
		};
		
		friend class ARX::Structure::Relation;
		
		class const_iterator;
		
		/**
		 * @brief An iterator class defining basic operations to iterate the Relation objects in a Structure.
		 * 
		 * This iterator class encapsules the internal form of Relation storage in a Structure.
         * By using this iterator class, which is what you have to do, you are safe from changes in implementation detail as the container used to store Relation objects.
         * Additionally it is the prefered way to make a visit to each Relation in a Structure.
		 **/
		class iterator
		{
		public:
			friend class ARX::Structure::const_iterator;
            
			/**
			 * @brief The constructor of a Relation iterator.
             * @param Iterator An iterator from the Structure's aggregated Relation container.
			 **/
			explicit iterator(std::map<std::string, ARX::Structure::Relation>::iterator Iterator);
			
			/**
			 * @brief Advances the iterator.
			 *
			 * Advances the Relation iterator.
             * It is not defined which will be the next Relation the iterator points to.
             * It is only assured that every Relation in the Structure will be passed once.
			 **/
			auto operator++() -> ARX::Structure::iterator &;
			
			/**
			 * @brief This will return the Relation that this iterator points to.
			 * 
			 * This is the non-const version of the operator*(), so it returns a non-const Relation.
			 **/
			auto operator*() -> ARX::Structure::Relation &;
			
			/**
			 * @brief This will return the Relation that this iterator points to.
			 * 
			 * This is the const version of the operator*(), thus the returned Relation is const as well.
			 **/
			auto operator*() const -> ARX::Structure::Relation const &;
			
			/**
			 * @brief The Relation accessor.
			 **/
			auto operator->() -> ARX::Structure::Relation *;
			
			/**
			 * @brief Tests two iterators for equality.
			 * @param Other An iterator to check against.
			 **/
			auto operator==(ARX::Structure::iterator const & Other) const -> bool;
			
			/**
			 * @brief Tests two iterators for inequality.
			 * @param Other An iterator to check against.
			 **/
			auto operator!=(ARX::Structure::iterator const & Other) const -> bool;
			
			/**
			 * @brief Tests two iterators for equality.
			 * @param Other A const iterator to check against.
			 **/
			auto operator==(ARX::Structure::const_iterator const & Other) const -> bool;
			
			/**
			 * @brief Tests two iterators for inequality.
			 * @param Other A const iterator to check against.
			 **/
			auto operator!=(ARX::Structure::const_iterator const & Other) const -> bool;
            
		private:
            /**
             * @brief The iterator into the Structure's relation container.
             **/
			std::map<std::string, ARX::Structure::Relation>::iterator m_Iterator;
            
		};
		
		/**
		 * @brief An iterator class defining basic operations to iterate the Relation objects in a Structure.
		 *
		 * This iterator class encapsules the internal form of Relation storage in a Structure. By using this iterator class, which is what you have to do, you are safe from changes in implementation detail as the container used to store Relation objects. Additionally it is the prefered way to make a visit to each Relation in a Structure.
		 **/
		class const_iterator
		{
		public:
			friend class ARX::Structure::iterator;
            
			/**
			 * @brief The constructor of a const Relation iterator.
			 * @param iRelation The STL iterator from the Structure's aggregated Relation container. The internal representation of the iterator.
			 *
			 * This constructor creates a const Relation iterator from an STL iterator.
			 **/
			explicit const_iterator(std::map<std::string, ARX::Structure::Relation>::const_iterator Iterator);
			
			/**
			 * @brief The constructor of a const Relation iterator from a non-const Relation iterator.
			 * @param iRelation The Relation iterator from the Structure's aggregated Relation container.
			 *
			 * This constructor creates a const Relation iterator from a non-const Relation iterator.
			 **/
			const_iterator(ARX::Structure::iterator Iterator);
			
			/**
			 * @brief Advances the iterator.
			 *
			 * Advances the Relation iterator. It is not defined which will be the next Relation the iterator points to. It is only assured that every Relation in the Structure will be passed once.
			 **/
			auto operator++() -> ARX::Structure::const_iterator &;
			
			/**
			 * @brief This will return the ARX::Relation that this iterator points to.
			 * 
			 * This is the const version of the operator*() thus the returned Relation is const as well.
			 **/
			auto operator*() const -> ARX::Structure::Relation const &;
			
			/**
			 * @brief The Relation accessor.
			 **/
			auto operator->() -> ARX::Structure::Relation const *;
			
			/**
			 * @brief Tests two iterators for equality.
			 * @param iRelation The iterator to check against.
			 **/
			auto operator==(ARX::Structure::const_iterator const & Other) const -> bool;
			
			/**
			 * @brief Tests two iterators for inequality.
			 * @param iRelation The iterator to check against.
			 **/
			auto operator!=(ARX::Structure::const_iterator const & Other) const -> bool;
            
		private:
			/**
			 * @brief The internal STL iterator.
			 * 
			 * This is an iterator into a Structure's internal Relation map.
			 **/
			std::map<std::string, ARX::Structure::Relation>::const_iterator m_Iterator;
            
		};
		
		typedef std::map<std::string, ARX::Structure::Relation>::size_type size_type;
		
		/**
		 * @brief This constructor binds the structure to a specific ARX::Item.
		 * @param Item The ARX::Item that this structure is linked to.
		 **/
		explicit Structure(ARX::Item & Item);
		
		/**
		 * @brief Adds an unresolved item reference to a relation.
		 * @param Relation The relation that is changed with this call.
		 * @param ItemIdentifier An item identifier that is to be added to the relation.
		 * @return A boolean value indicating whether the operation has been performed successfully.
		 * 
		 * This function will add the item identifier @a ItemIdentifier to the relation identified by @a Relation.
		 * 
		 * There are two things to consider:
		 * - The item identifier may not be invalid, i.e. equal to g_InvalidItemIdentifier.
		 * - You cannot change the "reference" relation with this function.
		 * 
		 * Violating one of these conditions will abort the call and return without having done anything.
		 * 
		 * This function will create the relation if it does not exist yet and the unresolved reference with the item identifier @a ItemIdentifier will be the only member of this new relation.
		 **/
		auto Add(std::uint32_t ItemIdentifier, std::string const & Relation = "child") -> bool;
		
		/**
		 * @brief Deletes an unresolved item reference from a relation.
		 * @param Relation The relation that is changed with this call.
		 * @param ItemIdentifier The item identifier that is tried to be deleted from the relation.
		 * @return A boolean value indicating whether the operation has been performed successfully.
		 * 
		 * This function will delete the unresolved item reference referring to an item with item identifier @a ItemIdentifier from the relation indicated by @a Relation.
		 * 
		 * If the unresolved item reference is not found in the relation the function may issue a second search.
		 * -# Given the item that @a this Structure belongs to is registered at a library, the function will try to resolve the unique ID with ARX::Library::pGetItem() (complexity in O(log(n))). If a resolved item reference to an item with this unique id is in the relation it is also found via pGetItem() and can and will be deleted from the relation. If pGetItem does not find the item there will be no such reference in the relation.
		 * -# Given the item that @a this Structure belongs to is not registered at a library, the function will iterate through the resolved item references in the relation trying to find one that refers to an item with this unique id.
		 * 
		 * There are two things to consider:
		 * - The item identifier may not be invalid, i.e. equal to g_u4InvalidItemIdentifier.
		 * - You can not modify the relation "reference" with this function.
		 * 
		 * Any atempt to do so as well as deleting from a non-existent relation will be silently ignored.
		 * 
		 * If the deleted item reference was the last one in the relation so that the relation @a sRelation is now empty it is deleted from the relations container.
		 **/
		auto Delete(std::uint32_t ItemIdentifier, std::string const & Relation = "child") -> bool;
		
		/**
		 * @brief The const accessor for constant relations.
		 * @param sRelation The relation's name.
		 * @throw std::out_of_range If a relation with the name @a sRelation does not exist in the structure.
		 * 
		 * This function returns a reference to a ARX::Structure::Relation object which you can use to query the content of this relation.
		 **/
		auto GetRelation(std::string const & Relation) const -> ARX::Structure::Relation const &;
		
		/**
		 * @brief The accessor for relations.
		 * @param sRelation The relation's name.
		 * @throw std::out_of_range If a relation with the name @a sRelation does not exist in the structure.
		 * 
		 * This function returns a reference to a ARX::Structure::Relation object which you can use to query and modify the content of this relation.
		 **/
		auto GetRelation(std::string const & Relation) -> ARX::Structure::Relation &;
		
		/**
		 * @brief Query whether a paricular relation exists.
		 * @param sRelation The name of the relation.
		 **/
		auto HasRelation(std::string const & Relation) const -> bool;
		
		/**
		 * @brief Returns the number of Relations in the Structure.
		 **/
		auto size() const -> size_type;
		
		/**
		 * @brief Returns the first Relation iterator of the Structure.
		 **/
		auto begin() -> ARX::Structure::iterator;
		
		/**
		 * @brief Returns the end Relation iterator of the Structure.
		 **/
		auto end() -> ARX::Structure::iterator;
		
		/**
		 * @brief Returns the first const Relation iterator of the Structure.
		 **/
		auto begin() const -> ARX::Structure::const_iterator;
		
		/**
		 * @brief Returns the end const Relation iterator of the Structure.
		 **/
		auto end() const -> ARX::Structure::const_iterator;
        
	private:
		friend auto ARX::operator<<(ARX::Buffer & Buffer, ARX::Structure const & Structure) -> ARX::Buffer &;
		
		/**
		 * @brief Writes the Structure information to a Buffer.
		 * @param Buffer The Buffer that will receive the structural information.
		 * 
		 * This function simply appends the structure information to the @a Buffer.
		 **/
		auto m_WriteToBuffer(ARX::Buffer & Buffer) const -> void;
		
		auto m_ReadFromStream(std::uint32_t StructureDataLength, std::istream & IStream) -> void;
		
		auto m_RemoveRelation(ARX::Structure::Relation * Relation) -> void;
        
		ARX::Item & m_Item;
		std::map<std::string, ARX::Structure::Relation> m_Relations;
        
	};
}

#endif
