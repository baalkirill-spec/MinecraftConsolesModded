#pragma once

#include "..\\Minecraft.World\\File.h"

#include <memory>
#include <string>

class Player;

class CustomSkinManager
{
public:
	explicit CustomSkinManager(const File& workingDirectory);

	void initialize(const std::wstring& configuredRelativePath);
	void applyToPlayer(const std::shared_ptr<Player>& player) const;
	const File& getSkinsDirectory() const;
	bool hasLoadedSkin() const;
	const std::wstring& getLoadedSkinPath() const;

private:
	File m_skinsDirectory;
	std::wstring m_loadedSkinPath;
	bool m_hasLoadedSkin;

	void ensureSkinsDirectory() const;
	bool loadSkinFile(const File& skinFile);
	static bool ReadBytes(const File& file, std::vector<unsigned char>& outBytes);
};
