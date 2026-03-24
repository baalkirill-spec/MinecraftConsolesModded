#pragma once

#include <cstdint>
#include <string>
#include <vector>

class SimpleZipReader
{
public:
	static bool ReadTextFile(const std::wstring& archivePath, const std::wstring& entryNameOrSuffix, std::string& outText);

private:
	struct CentralDirectoryEntry
	{
		std::wstring fileName;
		uint16_t compressionMethod = 0;
		uint32_t compressedSize = 0;
		uint32_t uncompressedSize = 0;
		uint32_t localHeaderOffset = 0;
		uint16_t generalPurposeFlag = 0;
	};

	static bool ReadAllBytes(const std::wstring& path, std::vector<uint8_t>& outBytes);
	static bool FindEntry(const std::vector<uint8_t>& bytes, const std::wstring& entryNameOrSuffix, CentralDirectoryEntry& outEntry);
	static bool ExtractEntry(const std::vector<uint8_t>& bytes, const CentralDirectoryEntry& entry, std::vector<uint8_t>& outBytes);
	static bool DecompressDeflate(const uint8_t* compressedData, size_t compressedSize, size_t uncompressedSize, std::vector<uint8_t>& outBytes);
	static uint16_t ReadUInt16(const std::vector<uint8_t>& bytes, size_t offset);
	static uint32_t ReadUInt32(const std::vector<uint8_t>& bytes, size_t offset);
};
