#pragma once

#include "ModInfo.h"
#include "..\\Minecraft.World\\File.h"

#include <unordered_map>
#include <vector>

class ModManager
{
public:
	struct ScanDiagnostic
	{
		std::wstring sourcePath;
		std::wstring severity;
		std::wstring message;
	};

	explicit ModManager(const File& workingDirectory);

	void initialize();
	void refresh();
	const std::vector<ModInfo>& getMods() const;
	const std::vector<ModInfo::DataDefinition>& getDataDefinitions() const;
	const std::vector<ScanDiagnostic>& getScanDiagnostics() const;
	const File& getModsDirectory() const;
	std::wstring resolveFolderAssetOverride(const std::wstring& relativePath) const;

private:
	File m_modsDirectory;
	std::vector<ModInfo> m_mods;
	std::vector<ModInfo::DataDefinition> m_dataDefinitions;
	std::vector<ScanDiagnostic> m_scanDiagnostics;
	mutable std::unordered_map<std::wstring, std::wstring> m_assetOverrideCache;

	void ensureModsDirectory() const;
	void scanMods();
	bool loadFolderMod(const File& path, ModInfo& outInfo) const;
	bool loadZipMod(const File& path, ModInfo& outInfo) const;
	bool loadFolderDataDefinitions(const File& path, ModInfo& outInfo) const;
	static bool ParseManifest(const std::string& manifestText, ModInfo& outInfo);
	static void CollectDataDefinitionsRecursive(const File& currentPath, const std::wstring& relativePath, ModInfo& outInfo);
	static bool ReadJsonFile(const File& file, std::string& outText);
	static std::wstring DeriveFallbackId(const File& path);
	static std::wstring ExtractJsonString(const std::string& json, const std::string& key);
	static std::vector<std::wstring> ExtractJsonStringArray(const std::string& json, const std::string& key);
	static bool ValidateAndNormalizeModId(const std::wstring& input, std::wstring& outNormalized);
	static bool ValidateResourcePathSegment(const std::wstring& input);
	static bool ValidateDefinitionPath(const std::wstring& input);
	static std::wstring NormalizePathSeparators(const std::wstring& input);
	static std::wstring NormalizeAssetOverridePath(const std::wstring& input);
	static std::wstring TrimAsciiWhitespace(const std::string& text);
	static bool EndsWithIgnoreCase(const std::wstring& value, const std::wstring& suffix);
};
