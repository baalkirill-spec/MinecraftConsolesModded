#include "stdafx.h"
#include "ModManager.h"
#include "SimpleZipReader.h"
#include "..\Minecraft.World\StringHelpers.h"

#include <algorithm>
#include <cwctype>
#include <fstream>

namespace
{
	constexpr const wchar_t* kManifestFileName = L"mod.json";

	std::string ReadFileToString(const File& file)
	{
		std::ifstream stream(wstringtochararray(file.getPath()), std::ios::binary);
		if (!stream.good())
		{
			return std::string();
		}

		stream.seekg(0, std::ios::end);
		const std::streamoff fileSize = stream.tellg();
		if (fileSize <= 0)
		{
			return std::string();
		}

		std::string output(static_cast<size_t>(fileSize), '\0');
		stream.seekg(0, std::ios::beg);
		stream.read(output.data(), fileSize);
		return output;
	}

	void AddUniquePath(std::vector<std::wstring>& paths, const std::wstring& path)
	{
		if (path.empty())
		{
			return;
		}

		for (const std::wstring& existingPath : paths)
		{
			if (toLower(existingPath) == toLower(path))
			{
				return;
			}
		}

		paths.push_back(path);
	}
}

ModManager::ModManager(const File& workingDirectory)
	: m_modsDirectory(workingDirectory, L"mods")
{
}

void ModManager::initialize()
{
	ensureModsDirectory();
	scanMods();
}

const std::vector<ModInfo>& ModManager::getMods() const
{
	return m_mods;
}

const File& ModManager::getModsDirectory() const
{
	return m_modsDirectory;
}

std::wstring ModManager::resolveFolderAssetOverride(const std::wstring& relativePath) const
{
	static const std::wstring candidatePrefixes[] =
	{
		L"",
		L"textures",
		L"minecraft",
		L"minecraft\textures"
	};

	for (auto it = m_mods.rbegin(); it != m_mods.rend(); ++it)
	{
		for (const std::wstring& assetRoot : it->assetRoots)
		{
			for (const std::wstring& prefix : candidatePrefixes)
			{
				const std::wstring relativeCandidate = prefix.empty() ? relativePath : (prefix + L"\\" + relativePath);
				File candidate(assetRoot, relativeCandidate);
				if (candidate.exists() && candidate.isFile())
				{
					return candidate.getPath();
				}
			}
		}
	}
	return L"";
}

void ModManager::ensureModsDirectory() const
{
	if (!m_modsDirectory.exists())
	{
		m_modsDirectory.mkdirs();
		app.DebugPrintf("Created mods directory at %ls\n", m_modsDirectory.getPath().c_str());
	}
}

void ModManager::scanMods()
{
	m_mods.clear();
	std::vector<File*>* files = m_modsDirectory.listFiles();
	if (files == nullptr)
	{
		return;
	}

	for (File* file : *files)
	{
		if (file == nullptr)
		{
			continue;
		}

		const std::wstring name = file->getName();
		if (name == L"." || name == L"..")
		{
			delete file;
			continue;
		}

		ModInfo info;
		bool loaded = false;
		if (file->isDirectory())
		{
			loaded = loadFolderMod(*file, info);
		}
		else if (file->isFile() && EndsWithIgnoreCase(name, L".zip"))
		{
			loaded = loadZipMod(*file, info);
		}

		if (loaded)
		{
			app.DebugPrintf("ModManager: found %ls (%ls) v%ls at %ls\n",
				info.getDisplayName().c_str(),
				info.getSourceTypeName().c_str(),
				info.version.empty() ? L"unknown" : info.version.c_str(),
				info.sourcePath.c_str());
			m_mods.push_back(info);
		}

		delete file;
	}

	delete files;
	app.DebugPrintf("ModManager: %d mod(s) discovered\n", static_cast<int>(m_mods.size()));
}

bool ModManager::loadFolderMod(const File& path, ModInfo& outInfo) const
{
	outInfo.sourcePath = path.getPath();
	outInfo.sourceType = ModInfo::SourceType::Folder;
	outInfo.id = DeriveFallbackId(path);
	outInfo.name = path.getName();
	outInfo.version = L"0.0.0";
	outInfo.description = L"Folder mod without mod.json";

	// Legacy root-first lookup keeps simple phase-1 folder mods working even before a full resource pack style mount exists.
	AddUniquePath(outInfo.assetRoots, path.getPath());

	const File manifest(path, kManifestFileName);
	if (manifest.exists() && manifest.isFile())
	{
		const std::string manifestText = ReadFileToString(manifest);
		outInfo.manifestPath = manifest.getPath();
		outInfo.hasManifest = ParseManifest(manifestText, outInfo);
	}

	const File assets(path, L"assets");
	if (assets.exists() && assets.isDirectory())
	{
		AddUniquePath(outInfo.assetRoots, assets.getPath());
	}

	const File data(path, L"data");
	if (data.exists() && data.isDirectory())
	{
		AddUniquePath(outInfo.dataRoots, data.getPath());
	}

	return true;
}

bool ModManager::loadZipMod(const File& path, ModInfo& outInfo) const
{
	outInfo.sourcePath = path.getPath();
	outInfo.sourceType = ModInfo::SourceType::ZipArchive;
	outInfo.id = DeriveFallbackId(path);
	outInfo.name = path.getName();
	outInfo.version = L"0.0.0";
	outInfo.description = L"Zip mod without mod.json";

	std::string manifestText;
	if (!SimpleZipReader::ReadTextFile(path.getPath(), kManifestFileName, manifestText))
	{
		app.DebugPrintf("ModManager: skipping zip %ls because no readable mod.json was found\n", path.getPath().c_str());
		return false;
	}

	outInfo.manifestPath = path.getPath() + L"!/mod.json";
	outInfo.hasManifest = ParseManifest(manifestText, outInfo);
	if (!outInfo.hasManifest)
	{
		app.DebugPrintf("ModManager: zip manifest %ls did not contain supported string metadata fields; using fallback id/name\n", outInfo.manifestPath.c_str());
	}

	// Phase 1 keeps folder mods as the only supported runtime asset override path.
	return true;
}

bool ModManager::ParseManifest(const std::string& manifestText, ModInfo& outInfo)
{
	const std::wstring id = ExtractJsonString(manifestText, "id");
	const std::wstring name = ExtractJsonString(manifestText, "name");
	const std::wstring version = ExtractJsonString(manifestText, "version");
	const std::wstring description = ExtractJsonString(manifestText, "description");

	if (!id.empty()) outInfo.id = id;
	if (!name.empty()) outInfo.name = name;
	if (!version.empty()) outInfo.version = version;
	if (!description.empty()) outInfo.description = description;
	return !id.empty() || !name.empty() || !version.empty() || !description.empty();
}

std::wstring ModManager::DeriveFallbackId(const File& path)
{
	std::wstring fallback = toLower(path.getName());
	const size_t dot = fallback.find_last_of(L'.');
	if (dot != std::wstring::npos)
	{
		fallback = fallback.substr(0, dot);
	}
	for (wchar_t& ch : fallback)
	{
		if (!(iswalnum(ch) || ch == L'_' || ch == L'-'))
		{
			ch = L'_';
		}
	}
	return fallback;
}

std::wstring ModManager::ExtractJsonString(const std::string& json, const std::string& key)
{
	const std::string quotedKey = std::string("\"") + key + "\"";
	const size_t keyPos = json.find(quotedKey);
	if (keyPos == std::string::npos)
	{
		return L"";
	}

	const size_t colonPos = json.find(':', keyPos + quotedKey.size());
	if (colonPos == std::string::npos)
	{
		return L"";
	}

	size_t valueStart = colonPos + 1;
	while (valueStart < json.size() && isspace(static_cast<unsigned char>(json[valueStart])))
	{
		++valueStart;
	}
	if (valueStart >= json.size() || json[valueStart] != '"')
	{
		return L"";
	}
	++valueStart;

	std::string value;
	for (size_t i = valueStart; i < json.size(); ++i)
	{
		if (json[i] == '\\' && i + 1 < json.size())
		{
			value.push_back(json[i + 1]);
			++i;
			continue;
		}
		if (json[i] == '"')
		{
			break;
		}
		value.push_back(json[i]);
	}

	return convStringToWstring(value);
}

bool ModManager::EndsWithIgnoreCase(const std::wstring& value, const std::wstring& suffix)
{
	if (value.size() < suffix.size())
	{
		return false;
	}
	return toLower(value.substr(value.size() - suffix.size())) == toLower(suffix);
}
