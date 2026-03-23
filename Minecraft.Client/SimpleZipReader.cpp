#include "stdafx.h"
#include "SimpleZipReader.h"
#include "..\\Minecraft.World\\StringHelpers.h"

#include <algorithm>
#include <fstream>

#ifdef _WINDOWS64
#include <compressapi.h>
#endif

namespace
{
	constexpr uint32_t kEndOfCentralDirectorySignature = 0x06054B50;
	constexpr uint32_t kCentralDirectoryFileHeaderSignature = 0x02014B50;
	constexpr uint32_t kLocalFileHeaderSignature = 0x04034B50;
	constexpr size_t kZipEndRecordMinSize = 22;
	constexpr size_t kMaxZipCommentSize = 0xFFFF;

	std::wstring NormalizeZipPath(const std::wstring& value)
	{
		std::wstring result = toLower(value);
		std::replace(result.begin(), result.end(), L'\\', L'/');
		return result;
	}
}

bool SimpleZipReader::ReadTextFile(const std::wstring& archivePath, const std::wstring& entryNameOrSuffix, std::string& outText)
{
	std::vector<uint8_t> archiveBytes;
	if (!ReadAllBytes(archivePath, archiveBytes))
	{
		return false;
	}

	CentralDirectoryEntry entry;
	if (!FindEntry(archiveBytes, entryNameOrSuffix, entry))
	{
		return false;
	}

	std::vector<uint8_t> entryBytes;
	if (!ExtractEntry(archiveBytes, entry, entryBytes))
	{
		return false;
	}

	outText.assign(reinterpret_cast<const char*>(entryBytes.data()), entryBytes.size());
	return true;
}

bool SimpleZipReader::ReadAllBytes(const std::wstring& path, std::vector<uint8_t>& outBytes)
{
	std::ifstream stream(wstringtochararray(path), std::ios::binary);
	if (!stream.good())
	{
		return false;
	}

	stream.seekg(0, std::ios::end);
	std::streamoff fileSize = stream.tellg();
	if (fileSize <= 0)
	{
		outBytes.clear();
		return false;
	}

	outBytes.resize(static_cast<size_t>(fileSize));
	stream.seekg(0, std::ios::beg);
	stream.read(reinterpret_cast<char*>(outBytes.data()), fileSize);
	return stream.good();
}

bool SimpleZipReader::FindEntry(const std::vector<uint8_t>& bytes, const std::wstring& entryNameOrSuffix, CentralDirectoryEntry& outEntry)
{
	if (bytes.size() < kZipEndRecordMinSize)
	{
		return false;
	}

	const size_t searchStart = bytes.size() > (kZipEndRecordMinSize + kMaxZipCommentSize)
		? bytes.size() - (kZipEndRecordMinSize + kMaxZipCommentSize)
		: 0;

	size_t eocdOffset = SIZE_MAX;
	for (size_t offset = bytes.size() - kZipEndRecordMinSize + 1; offset-- > searchStart; )
	{
		if (ReadUInt32(bytes, offset) == kEndOfCentralDirectorySignature)
		{
			eocdOffset = offset;
			break;
		}
		if (offset == 0)
		{
			break;
		}
	}

	if (eocdOffset == SIZE_MAX)
	{
		return false;
	}

	const uint16_t entryCount = ReadUInt16(bytes, eocdOffset + 10);
	const uint32_t centralDirectoryOffset = ReadUInt32(bytes, eocdOffset + 16);
	if (centralDirectoryOffset >= bytes.size())
	{
		return false;
	}

	const std::wstring wanted = NormalizeZipPath(entryNameOrSuffix);
	bool found = false;
	std::wstring bestName;
	size_t offset = centralDirectoryOffset;

	for (uint16_t i = 0; i < entryCount && offset + 46 <= bytes.size(); ++i)
	{
		if (ReadUInt32(bytes, offset) != kCentralDirectoryFileHeaderSignature)
		{
			break;
		}

		const uint16_t fileNameLength = ReadUInt16(bytes, offset + 28);
		const uint16_t extraFieldLength = ReadUInt16(bytes, offset + 30);
		const uint16_t fileCommentLength = ReadUInt16(bytes, offset + 32);
		const size_t headerSize = 46 + fileNameLength + extraFieldLength + fileCommentLength;
		if (offset + headerSize > bytes.size())
		{
			break;
		}

		std::string utf8Name(reinterpret_cast<const char*>(bytes.data() + offset + 46), fileNameLength);
		std::wstring fileName = convStringToWstring(utf8Name);
		std::wstring normalizedFileName = NormalizeZipPath(fileName);
		const bool exactMatch = normalizedFileName == wanted;
		const bool suffixMatch = normalizedFileName.size() > wanted.size() &&
			normalizedFileName.compare(normalizedFileName.size() - wanted.size(), wanted.size(), wanted) == 0 &&
			normalizedFileName[normalizedFileName.size() - wanted.size() - 1] == L'/';

		if (exactMatch || suffixMatch)
		{
			if (!found || normalizedFileName.size() < bestName.size())
			{
				bestName = normalizedFileName;
				outEntry.fileName = fileName;
				outEntry.compressionMethod = ReadUInt16(bytes, offset + 10);
				outEntry.generalPurposeFlag = ReadUInt16(bytes, offset + 8);
				outEntry.compressedSize = ReadUInt32(bytes, offset + 20);
				outEntry.uncompressedSize = ReadUInt32(bytes, offset + 24);
				outEntry.localHeaderOffset = ReadUInt32(bytes, offset + 42);
				found = true;
			}
		}

		offset += headerSize;
	}

	return found;
}

bool SimpleZipReader::ExtractEntry(const std::vector<uint8_t>& bytes, const CentralDirectoryEntry& entry, std::vector<uint8_t>& outBytes)
{
	if (entry.localHeaderOffset + 30 > bytes.size())
	{
		return false;
	}
	if (ReadUInt32(bytes, entry.localHeaderOffset) != kLocalFileHeaderSignature)
	{
		return false;
	}

	const uint16_t fileNameLength = ReadUInt16(bytes, entry.localHeaderOffset + 26);
	const uint16_t extraFieldLength = ReadUInt16(bytes, entry.localHeaderOffset + 28);
	const size_t dataOffset = entry.localHeaderOffset + 30 + fileNameLength + extraFieldLength;
	if (dataOffset + entry.compressedSize > bytes.size())
	{
		return false;
	}

	const uint8_t* compressedData = bytes.data() + dataOffset;
	if (entry.compressionMethod == 0)
	{
		outBytes.assign(compressedData, compressedData + entry.compressedSize);
		return true;
	}

	if (entry.compressionMethod == 8)
	{
		return DecompressDeflate(compressedData, entry.compressedSize, entry.uncompressedSize, outBytes);
	}

	app.DebugPrintf("SimpleZipReader: unsupported compression method %d\n", entry.compressionMethod);
	return false;
}

bool SimpleZipReader::DecompressDeflate(const uint8_t* compressedData, size_t compressedSize, size_t uncompressedSize, std::vector<uint8_t>& outBytes)
{
#ifdef _WINDOWS64
	outBytes.resize(uncompressedSize);
	COMPRESSOR_HANDLE decompressor = nullptr;
	if (!CreateDecompressor(COMPRESS_ALGORITHM_DEFLATE, nullptr, &decompressor))
	{
		return false;
	}

	SIZE_T finalSize = 0;
	const BOOL ok = Decompress(decompressor,
		compressedData,
		compressedSize,
		outBytes.data(),
		uncompressedSize,
		&finalSize);
	CloseDecompressor(decompressor);
	if (!ok)
	{
		outBytes.clear();
		return false;
	}
	outBytes.resize(finalSize);
	return true;
#else
	(void)compressedData;
	(void)compressedSize;
	(void)uncompressedSize;
	(void)outBytes;
	return false;
#endif
}

uint16_t SimpleZipReader::ReadUInt16(const std::vector<uint8_t>& bytes, size_t offset)
{
	return static_cast<uint16_t>(bytes[offset]) |
		(static_cast<uint16_t>(bytes[offset + 1]) << 8);
}

uint32_t SimpleZipReader::ReadUInt32(const std::vector<uint8_t>& bytes, size_t offset)
{
	return static_cast<uint32_t>(bytes[offset]) |
		(static_cast<uint32_t>(bytes[offset + 1]) << 8) |
		(static_cast<uint32_t>(bytes[offset + 2]) << 16) |
		(static_cast<uint32_t>(bytes[offset + 3]) << 24);
}
