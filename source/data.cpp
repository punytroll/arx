/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#include <cstdint>

/// optional headers
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#ifdef HAVE_BZLIB_H
#include <bzlib.h>
#endif

#include <arx/data.h>

/// default compression
#ifdef HAVE_BZLIB_H
ARX::Data::Compression ARX::Data::m_DefaultCompression = BZLIB;
#elif HAVE_ZLIB_H
ARX::Data::Compression ARX::Data::m_DefaultCompression = ZLIB_9;
#else
ARX::Data::Compression ARX::Data::m_DefaultCompression = NONE;
#endif

auto ARX::Data::Decompress() -> void
{
	switch(m_CompressionType)
	{
	case ARX::Data::NONE:
		{
			break;
		}
	case ARX::Data::ZLIB_0:
	case ARX::Data::ZLIB_1:
	case ARX::Data::ZLIB_2:
	case ARX::Data::ZLIB_3:
	case ARX::Data::ZLIB_4:
	case ARX::Data::ZLIB_5:
	case ARX::Data::ZLIB_6:
	case ARX::Data::ZLIB_7:
	case ARX::Data::ZLIB_8:
	case ARX::Data::ZLIB_9:
		{
#ifdef HAVE_ZLIB_H
			auto DecompressedData = new ARX::Data::value_type[m_DecompressedLength];
			z_stream zStream;
			int iReturn = 0;
			
			zStream.zalloc = nullptr;
			zStream.zfree = nullptr;
			zStream.next_in = const_cast<std::uint8_t *>(reinterpret_cast<std::uint8_t const *>(GetBegin()));
			/** @todo Why not use u4GetCompressedLength() here? **/
			zStream.avail_in = GetLength();
			zStream.next_out = reinterpret_cast<std::uint8_t *>(DecompressedData);
			/** @todo Why not use u4GetDecompressedLength() here? **/
			zStream.avail_out = m_DecompressedLength;
			if((iReturn = inflateInit(&zStream)) != Z_OK)
			{
				delete[] DecompressedData;
				/** @todo Correctly specify the actual error that occured in zlib. **/
				throw zlib_error("TODO: specify!");
			}
			if((iReturn = inflate(&zStream, Z_FINISH)) != Z_STREAM_END)
			{
				delete[] DecompressedData;
				/** @todo Correctly specify the actual error that occured in zlib. **/
				throw zlib_error("TODO: specify!");
			}
			if((iReturn = inflateEnd(&zStream)) != Z_OK)
			{
				delete[] DecompressedData;
				/** @todo Correctly specify the actual error that occured in zlib. **/
				throw zlib_error("TODO: specify!");
			}
			SetLength(0);
			Insert(0, m_DecompressedLength, DecompressedData);
			delete[] DecompressedData;
			m_CompressionType = ARX::Data::NONE;
			m_CompressedLength = 0;
#else
			throw std::runtime_error{"Unsupported decompression method."};
#endif
			
			break;
		}
	case ARX::Data::BZLIB:
		{
#ifdef HAVE_BZLIB_H
			auto DecompressedData = new ARX::Data::value_type[m_DecompressedLength];
			bz_stream BZStream;
			
			BZStream.bzalloc = nullptr;
			BZStream.bzfree = nullptr;
			BZStream.opaque = 0;
			BZStream.next_in = const_cast<char *>(reinterpret_cast<char const *>(GetBegin()));
			/** @todo Why not use u4GetCompressedLength() here? **/
			BZStream.avail_in = GetLength();
			BZStream.next_out = reinterpret_cast<char *>(DecompressedData);
			/** @todo Why not use u4GetDecompressedLength() here? **/
			BZStream.avail_out = m_DecompressedLength;
			
			int iBZResult;
			
			if((iBZResult = BZ2_bzDecompressInit(&BZStream, 0, 0)) != BZ_OK)
			{
				delete[] DecompressedData;
				/** @todo Correctly specify the actual error that occured in bzlib. **/
				throw bzlib_error("TODO: specify!");
			}
			if((iBZResult = BZ2_bzDecompress(&BZStream)) != BZ_STREAM_END)
			{
				delete[] DecompressedData;
				/** @todo Correctly specify the actual error that occured in bzlib. **/
				throw bzlib_error("TODO: specify!");
			}
			if((iBZResult = BZ2_bzDecompressEnd(&BZStream)) != BZ_OK)
			{
				delete[] DecompressedData;
				/** @todo Correctly specify the actual error that occured in bzlib. **/
				throw bzlib_error("TODO: specify!");
			}
			SetLength(0);
			Insert(0, m_DecompressedLength, DecompressedData);
			delete[] DecompressedData;
			m_CompressionType = ARX::Data::NONE;
			m_CompressedLength = 0;
#else
			throw std::runtime_error{"Unsupported decompression method."};
#endif
			break;
		}
	}
}

auto ARX::Data::Compress(ARX::Data::Compression CompressionType) -> void
{
	if((IsCompressed() == true) || (IsFetched() == false))
	{
		return;
	}
	switch(CompressionType)
	{
	case ARX::Data::NONE:
		{
			break;
		}
	case ARX::Data::ZLIB_0:
	case ARX::Data::ZLIB_1:
	case ARX::Data::ZLIB_2:
	case ARX::Data::ZLIB_3:
	case ARX::Data::ZLIB_4:
	case ARX::Data::ZLIB_5:
	case ARX::Data::ZLIB_6:
	case ARX::Data::ZLIB_7:
	case ARX::Data::ZLIB_8:
	case ARX::Data::ZLIB_9:
		{
#ifdef HAVE_ZLIB_H
			auto CompressedData = new ARX::Data::value_type[static_cast<std::uint32_t>(GetDecompressedLength() * 1.001 + 12)];
			z_stream zStream;
			signed long slReturn = 0;
			
			zStream.zalloc = nullptr;
			zStream.zfree = nullptr;
			zStream.next_in = const_cast<unsigned char *>(reinterpret_cast<unsigned char const *>(GetBegin()));
			zStream.avail_in = GetDecompressedLength();
			zStream.next_out = reinterpret_cast<unsigned char *>(CompressedData);
			zStream.avail_out = static_cast<std::uint32_t>(GetDecompressedLength() * 1.001 + 12);
			if((slReturn = deflateInit(&zStream, CompressionType - ARX::Data::ZLIB_0)) != Z_OK)
			{
				delete[] CompressedData;
				/** @todo Correctly specify the actual error that occured in zlib. **/
				throw zlib_error("TODO: specify!");
			}
			if((slReturn = deflate(&zStream, Z_FINISH)) != Z_STREAM_END)
			{
				delete[] CompressedData;
				/** @todo Correctly specify the actual error that occured in zlib. **/
				throw zlib_error("TODO: specify!");
			}
			if((slReturn = deflateEnd(&zStream)) != Z_OK)
			{
				delete[] CompressedData;
				/** @todo Correctly specify the actual error that occured in zlib. **/
				throw zlib_error("TODO: specify!");
			}
			SetLength(0);
			Insert(0, zStream.total_out, CompressedData);
			delete[] CompressedData;
			m_CompressionType = CompressionType;
			m_CompressedLength = zStream.total_out;
#else
			throw std::runtime_error{"Unsupported compression method."};
#endif
			
			break;
		}
	case ARX::Data::BZLIB:
		{
#ifdef HAVE_BZLIB_H
			auto CompressedDataLength = static_cast<std::uint32_t>(GetDecompressedLength() * 1.01 + 600);
			auto CompressedData = new ARX::Data::value_type[CompressedDataLength];
			bz_stream BZStream;
			
			BZStream.bzalloc = nullptr;
			BZStream.bzfree = nullptr;
			BZStream.opaque = 0;
			BZStream.next_in = const_cast<char *>(reinterpret_cast<char const *>(GetBegin()));
			BZStream.avail_in = GetDecompressedLength();
			BZStream.next_out = reinterpret_cast<char *>(CompressedData);
			BZStream.avail_out = CompressedDataLength;
			
			int iBZResult;
			
			if((iBZResult = BZ2_bzCompressInit(&BZStream, 9, 0, 0)) != BZ_OK)
			{
				delete[] CompressedData;
				/** @todo Correctly specify the actual error that occured in bzlib. **/
				throw bzlib_error("TODO: specify!");
			}
			if((iBZResult = BZ2_bzCompress(&BZStream, BZ_FINISH)) != BZ_STREAM_END)
			{
				delete[] CompressedData;
				/** @todo Correctly specify the actual error that occured in bzlib. **/
				throw bzlib_error("TODO: specify!");
			}
			if((iBZResult = BZ2_bzCompressEnd(&BZStream)) != BZ_OK)
			{
				delete[] CompressedData;
				/** @todo Correctly specify the actual error that occured in bzlib. **/
				throw bzlib_error("TODO: specify!");
			}
			SetLength(0);
			Insert(0, BZStream.total_out_lo32, CompressedData);
			delete[] CompressedData;
			m_CompressionType = CompressionType;
			m_CompressedLength = BZStream.total_out_lo32;
#else
			throw std::runtime_error{"Unsupported compression method."};
#endif
			break;
		}
	}
}

auto ARX::Data::IsCompressed() const -> bool
{
	return m_CompressionType != ARX::Data::NONE;
}

auto ARX::Data::IsDecompressed() const -> bool
{
	return m_CompressionType == ARX::Data::NONE;
}

auto ARX::Data::GetCompression() const -> ARX::Data::Compression
{
	return m_CompressionType;
}

auto ARX::Data::GetDecompressedLength() const -> std::uint32_t
{
	if((IsFetched() == true) && (IsDecompressed() == true))
	{
		return GetLength();
	}
	else
	{
		return m_DecompressedLength;
	}
}

auto ARX::Data::GetCompressedLength() const -> std::uint32_t
{
	if((IsFetched() == true) && (IsCompressed() == true))
	{
		return GetLength();
	}
	else
	{
		return m_CompressedLength;
	}
}

auto ARX::Data::Fetch() -> bool
{
	if(m_Fetched == false)
	{
		m_Fetched = m_Fetch(m_Offset, ((m_CompressionType == ARX::Data::NONE) ? (m_DecompressedLength) : (m_CompressedLength)));
		
		return m_Fetched;
	}
	else
	{
		return false;
	}
}

auto ARX::Data::Unfetch() -> void
{
	if(IsFetched() == true)
	{
		Delete(0, GetLength());
		m_Fetched = false;
	}
}

auto ARX::Data::SetFetchInformation(std::uint32_t Offset, ARX::Data::Compression CompressionType, std::uint32_t DecompressedLength, std::uint32_t CompressedLength) -> void
{
	m_Offset = Offset;
	m_CompressionType = CompressionType;
	m_DecompressedLength = DecompressedLength;
	m_CompressedLength = CompressedLength;
	m_Fetched = false;
}

auto ARX::Data::IsFetched() const -> bool
{
	return m_Fetched;
}

auto ARX::Data::m_Fetch([[maybe_unused]] std::uint32_t Offset, std::uint32_t Length) -> bool
{
	return Length == 0;
}
