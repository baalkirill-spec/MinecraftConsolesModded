#pragma once

#include <vector>
#include <string>

#include "..\\Minecraft.World\\File.h"

struct ModInfo
{
	std::wstring id;
	std::wstring name;
	std::wstring version;
	File source;
	bool isZip;
	bool enabled;
};

class ModManager
{
public:
	ModManager();

	void init(const File& workingDirectory);
	void reload();

	const std::vector<ModInfo>& getMods() const;
	const File& getModsDirectory() const;

private:
	File m_workingDirectory;
	File m_modsDirectory;
	std::vector<ModInfo> m_mods;

	void ensureModsDirectory();
	void scanMods();
	void addFolderMod(const File& file);
	void addZipMod(const File& file);
	bool hasExtension(const std::wstring& fileName, const std::wstring& extension) const;
};
