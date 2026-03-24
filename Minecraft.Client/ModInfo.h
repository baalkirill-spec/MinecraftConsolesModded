#pragma once

#include <string>
#include <vector>

struct ModInfo
{
	struct DataDefinition
	{
		enum class Type
		{
			Blocks,
			Items,
			Entities,
			Mechanics,
			WorldRules,
			Unknown,
		};

		Type type = Type::Unknown;
		std::wstring namespaceId;
		std::wstring pathId;
		std::wstring identifier;
		std::wstring sourcePath;
		std::wstring declaredId;
		std::string jsonText;
		bool valid = false;
		std::wstring validationMessage;

		std::wstring getTypeName() const;
	};

	enum class SourceType
	{
		Folder,
		ZipArchive,
	};

	std::wstring id;
	std::wstring name;
	std::wstring version;
	std::wstring description;
	std::wstring author;
	std::wstring sourcePath;
	std::wstring manifestPath;
	SourceType sourceType = SourceType::Folder;
	bool hasManifest = false;
	bool runtimeAssetOverrides = false;
	bool runtimeDataDiscovery = false;
	std::vector<std::wstring> assetRoots;
	std::vector<std::wstring> dataRoots;
	std::vector<std::wstring> authors;
	std::vector<std::wstring> warnings;
	std::vector<std::wstring> errors;
	std::vector<DataDefinition> dataDefinitions;

	std::wstring getDisplayName() const;
	std::wstring getSourceTypeName() const;
	bool hasIssues() const;
};
