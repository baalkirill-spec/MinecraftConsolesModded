#pragma once

#include <string>
#include <vector>

struct ModInfo
{
	enum class SourceType
	{
		Folder,
		ZipArchive,
	};

	std::wstring id;
	std::wstring name;
	std::wstring version;
	std::wstring description;
	std::wstring sourcePath;
	std::wstring manifestPath;
	SourceType sourceType = SourceType::Folder;
	bool hasManifest = false;
	std::vector<std::wstring> assetRoots;
	std::vector<std::wstring> dataRoots;

	std::wstring getDisplayName() const;
	std::wstring getSourceTypeName() const;
};
