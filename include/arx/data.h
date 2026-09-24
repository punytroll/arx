/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__DATA_H
#define ARX__INCLUDE__DATA_H

#include <arx/buffer.h>
#include <arx/common.h>

/**
 * @file data.h
 * 
 * Declares the interface of ARX::Data.
 **/

namespace ARX
{
	/**
	 * @brief A buffer with compression, decompression and external data referencing features.
	 *
	 * This is the component underlying the ARX::Item class.
     * It delivers the ability to compress and decompress the data.
     * Because of the public inhertitance from ARX::Buffer it is still possible to call Buffer::GetLength() but be aware that it will give you the state of the buffer which might not be what you want for two reasons:
	 * - the buffer content might be compressed or decompressed
	 * - the data might not be fetched yet, which makes ARX::Buffer::GetLength() return `0` since there is nothing in the buffer
	 * - the data might not be fetched yet, but you have written something in the buffer.
     *   ARX::Buffer::GetLength() will of course return the length of that data instead of the length of the Item's data, be it compressed or decompressed.
	 **/
	class Data : public ARX::Buffer
	{
	public:
		typedef enum
		{
			NONE = 0,
			ZLIB_0 = 1,
			ZLIB_1 = 2,
			ZLIB_2 = 3,
			ZLIB_3 = 4,
			ZLIB_4 = 5,
			ZLIB_5 = 6,
			ZLIB_6 = 7,
			ZLIB_7 = 8,
			ZLIB_8 = 9,
			ZLIB_9 = 10,
			BZLIB = 11
		} Compression;
		
		static ARX::Data::Compression m_DefaultCompression;
		
		/**
		 * @brief Decompresses the data of the buffer.
		 **/
		auto Decompress() -> void;
		
		/**
		 * @brief Compresses the data of the buffer.
		 **/
		auto Compress(ARX::Data::Compression Compression = ARX::Data::m_DefaultCompression) -> void;
		
		/**
		 * @brief Returns true if the data inside the buffer is compressed.
		 **/
		auto IsCompressed() const -> bool;
		
		/**
		 * @brief Returns true if the data inside the buffer is decompressed.
		 **/
		auto IsDecompressed() const -> bool;
		
		/**
		 * @brief Returns the compression information, which will equal the compression rate of the zlib library.
		 **/
		auto GetCompression() const -> ARX::Data::Compression;
		
		/**
		 * @brief Gets the length of the decompressed data.
		 **/
		auto GetDecompressedLength() const -> std::uint32_t;
		
		/**
		 * @brief Gets the length of the compressed data.
		 **/
		auto GetCompressedLength() const -> std::uint32_t;
		
		/**
		 * @brief This function requests the item's data at the appropriate data channel.
		 * @return Wether the request to fetch the data could be handed to a data channel.
		 * 
		 * Note that fetching the data does not automatically internalize it.
		 * 
		 * In order to only try to fetch the data m_URI must be valid and bIsFetched() and bIsFetching() must be false.
         * Otherwise the function will return false.
		 * 
		 * For asynchronous fetches the return value may be true because an ARX::DataChannel could be found but the actuall fetching may fail.
         * This information may be retrieved via ARX::Data::GetFetchStatus().
		 **/
		auto Fetch() -> bool;
		
		/**
		 * @brief Unfetching the data generally means deleting the data thus releasing the memory in the buffer.
		 * 
		 * This function will empty the data buffer and release the memory associated with it.
         * So why an extra function if this could be done with @a Buffers::Buffer::Delete()?
         * 
		 * Because if you delete the content of the buffer you change the data.
         * Hence you don't change the flag whether the data is fetched and will not be able the refetch the data using @a Fetch() again.
		 * Thus, this function requires the bIsFetched() to be true.
		 **/
		auto Unfetch() -> void;
		
		/**
		 * @brief Sets fetch information for the data object.
		 * @param Offset The offset of the fetchable data.
		 * @param Compression The state of the data.
         *                    If this value if greater than 0 the fake data is considered compressed.
		 * @param DecompressedLength The length of the data if in decompressed state.
         *                           Elsewise the value which @a ARX::Buffer::GetDecompressedLength() will return and this data will serve no internal functionality, so you may use it as you like.
		 * @param CompressedLength The length of the data if in compressed state.
         *                         Elsewise the value which ARX::Buffer::GetCompressedDataLength() will return and this data will serve no internal functionality, so you may use it as you like.
		 * 
		 * This function will set @a m_Fetched to false.
		 **/
		auto SetFetchInformation(std::uint32_t Offset, ARX::Data::Compression Compression, std::uint32_t DecompressedLength, std::uint32_t CompressedLength) -> void;
		
		/**
		 * @brief Returns wether the data is fetched.
		 **/
		auto IsFetched() const -> bool;
        
	protected:
		virtual auto m_Fetch(std::uint32_t Offset, std::uint32_t Length) -> bool;
        
	private:
		/**
		 * @brief A locator used to further describe the location to the fetcher.
		 * 
		 * After creating an Item this value is intentionally invalid.
		 **/
		std::uint32_t m_Offset{0};
		
		/**
		 * @brief The compressed state the data is currently in.
		 * 
		 * Is set to either 0 for to indicate no compression or some value greater than 0 to indicate the compression level of the data as in the zlib library.
		 **/
		ARX::Data::Compression m_CompressionType{ARX::Data::Compression::NONE};
		
		/**
		 * @brief The length of the data if decompressed.
		 **/
		std::uint32_t m_DecompressedLength{0};
		
		/**
		 * @brief The length of the data if compressed or 0.
		 * 
		 * Since the compressed size is not known as long as the data is decompressed this value is 0 whenever the data is decompressed.
		 **/
		std::uint32_t m_CompressedLength{0};
		
		/**
		 * @brief A status indicator showing the fetch status of the data.
		 **/
		bool m_Fetched{true};
        
	};
}

#endif
