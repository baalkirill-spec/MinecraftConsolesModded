#include "stdafx.h"
#include "CustomSkinManager.h"
#include "BufferedImage.h"
#include "..\\Minecraft.World\\StringHelpers.h"
#include "..\\Minecraft.World\\net.minecraft.world.entity.player.h"

#include <fstream>
#include <vector>

namespace
{
	constexpr DWORD kCustomSkinBitmask = 0x20;
	constexpr const wchar_t* kRuntimeSkinName = L"ugcskin00000020.png";
	constexpr size_t kMaxSkinFileBytes = 8 * 1024 * 1024;

	bool EndsWithIgnoreCase(const std::wstring& value, const std::wstring& suffix)
	{
		if (value.size() < suffix.size())
		{
			return false;
		}
		return toLower(value.substr(value.size() - suffix.size())) == toLower(suffix);
	}
}

CustomSkinManager::CustomSkinManager(const File& workingDirectory)
	: m_skinsDirectory(workingDirectory, L"skins"),
	  m_hasLoadedSkin(false)
{
}

void CustomSkinManager::initialize(const std::wstring& configuredRelativePath)
{
	ensureSkinsDirectory();
	std::wstring safeRelativePath = configuredRelativePath.empty() ? L"custom_skin.png" : configuredRelativePath;
	if (safeRelativePath.find(L"..") != std::wstring::npos || safeRelativePath.find(L":") != std::wstring::npos)
	{
		app.DebugPrintf("CustomSkinManager: rejected unsafe skin path %ls\n", safeRelativePath.c_str());
		safeRelativePath = L"custom_skin.png";
	}
	File skinFile(m_skinsDirectory, safeRelativePath);
	loadSkinFile(skinFile);
}

void CustomSkinManager::applyToPlayer(const std::shared_ptr<Player>& player) const
{
	if (!m_hasLoadedSkin || player == nullptr)
	{
		return;
	}
	player->setCustomSkin(MAKE_SKIN_BITMASK(false, kCustomSkinBitmask));
}

const File& CustomSkinManager::getSkinsDirectory() const
{
	return m_skinsDirectory;
}

bool CustomSkinManager::hasLoadedSkin() const
{
	return m_hasLoadedSkin;
}

const std::wstring& CustomSkinManager::getLoadedSkinPath() const
{
	return m_loadedSkinPath;
}

void CustomSkinManager::ensureSkinsDirectory() const
{
	if (!m_skinsDirectory.exists())
	{
		m_skinsDirectory.mkdirs();
		app.DebugPrintf("Created skins directory at %ls\n", m_skinsDirectory.getPath().c_str());
	}
}

bool CustomSkinManager::loadSkinFile(const File& skinFile)
{
	m_hasLoadedSkin = false;
	m_loadedSkinPath.clear();

	if (!skinFile.exists() || !skinFile.isFile())
	{
		return false;
	}

	if (!EndsWithIgnoreCase(skinFile.getName(), L".png"))
	{
		app.DebugPrintf("CustomSkinManager: rejected non-png skin file %ls\n", skinFile.getPath().c_str());
		return false;
	}

	std::vector<unsigned char> bytes;
	if (!ReadBytes(skinFile, bytes))
	{
		app.DebugPrintf("CustomSkinManager: failed to read %ls\n", skinFile.getPath().c_str());
		return false;
	}

	BufferedImage image(bytes.data(), static_cast<DWORD>(bytes.size()));
	if (image.getWidth() != 64 || image.getHeight() != 32)
	{
		app.DebugPrintf("CustomSkinManager: skin %ls has unsupported size %dx%d (expected classic 64x32 for this first phase)\n", skinFile.getPath().c_str(), image.getWidth(), image.getHeight());
		return false;
	}

	unsigned char* storedBytes = new unsigned char[bytes.size()];
	memcpy(storedBytes, bytes.data(), bytes.size());
	app.AddMemoryTextureFile(kRuntimeSkinName, storedBytes, static_cast<DWORD>(bytes.size()));
	m_loadedSkinPath = skinFile.getPath();
	m_hasLoadedSkin = true;
	app.DebugPrintf("CustomSkinManager: loaded custom skin from %ls\n", m_loadedSkinPath.c_str());
	return true;
}

bool CustomSkinManager::ReadBytes(const File& file, std::vector<unsigned char>& outBytes)
{
	std::ifstream stream(wstringtochararray(file.getPath()), std::ios::binary);
	if (!stream.good())
	{
		return false;
	}

	stream.seekg(0, std::ios::end);
	std::streamoff fileSize = stream.tellg();
	if (fileSize <= 0 || static_cast<size_t>(fileSize) > kMaxSkinFileBytes)
	{
		return false;
	}

	outBytes.resize(static_cast<size_t>(fileSize));
	stream.seekg(0, std::ios::beg);
	stream.read(reinterpret_cast<char*>(outBytes.data()), fileSize);
	return stream.good();
}
