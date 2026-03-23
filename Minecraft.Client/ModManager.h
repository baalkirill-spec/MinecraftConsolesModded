#pragma once

#include "ModInfo.h"
#include "..\\Minecraft.World\\File.h"

#include <vector>

class ModManager
{
public:
	explicit ModManager(const File& workingDirectory);

	void initialize();
	const std::vector<ModInfo>& getMods() const;
	const File& getModsDirectory() const;
	std::wstring resolveFolderAssetOverride(const std::wstring& relativePath) const;

private:
	File m_modsDirectory;
	std::vector<ModInfo> m_mods;

	void ensureModsDirectory() const;
	void scanMods();
	bool loadFolderMod(const File& path, ModInfo& outInfo) const;
	bool loadZipMod(const File& path, ModInfo& outInfo) const;
	static bool ParseManifest(const std::string& manifestText, ModInfo& outInfo);
	static std::wstring DeriveFallbackId(const File& path);
	static std::wstring ExtractJsonString(const std::string& json, const std::string& key);
	static bool EndsWithIgnoreCase(const std::wstring& value, const std::wstring& suffix);
};
