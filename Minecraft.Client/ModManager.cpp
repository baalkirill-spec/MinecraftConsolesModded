#include "stdafx.h"
#include "ModManager.h"
#include "SimpleZipReader.h"
#include "..\Minecraft.World\StringHelpers.h"

#include <algorithm>
#include <cwctype>
#include <fstream>
#include <set>

namespace
{
	constexpr const wchar_t* kManifestFileName = L"mod.json";
	constexpr const wchar_t* kAssetsDirectoryName = L"assets";
	constexpr const wchar_t* kDataDirectoryName = L"data";
	constexpr size_t kMaxJsonFileBytes = 256 * 1024;

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

	void AddUniqueString(std::vector<std::wstring>& values, const std::wstring& value)
	{
		if (value.empty())
		{
			return;
		}

		for (const std::wstring& existingValue : values)
		{
			if (toLower(existingValue) == toLower(value))
			{
				return;
			}
		}

		values.push_back(value);
	}

	std::wstring ToLowerCopy(const std::wstring& value)
	{
		return toLower(value);
	}

	std::vector<std::wstring> SplitPath(const std::wstring& value)
	{
		std::vector<std::wstring> segments;
		std::wstring current;
		for (wchar_t ch : value)
		{
			if (ch == L'\\' || ch == L'/')
			{
				if (!current.empty())
				{
					segments.push_back(current);
					current.clear();
				}
			}
			else
			{
				current.push_back(ch);
			}
		}
		if (!current.empty())
		{
			segments.push_back(current);
		}
		return segments;
	}

	std::wstring JoinPath(const std::vector<std::wstring>& segments, size_t startIndex, size_t endIndexExclusive)
	{
		std::wstring output;
		for (size_t i = startIndex; i < endIndexExclusive; ++i)
		{
			if (!output.empty())
			{
				output += L"/";
			}
			output += segments[i];
		}
		return output;
	}

	std::wstring RemoveJsonExtension(const std::wstring& value)
	{
		if (value.size() >= 5 && ToLowerCopy(value.substr(value.size() - 5)) == L".json")
		{
			return value.substr(0, value.size() - 5);
		}
		return value;
	}

	bool LooksLikeSimpleVersion(const std::wstring& version)
	{
		if (version.empty())
		{
			return false;
		}

		for (wchar_t ch : version)
		{
			if ((ch >= L'0' && ch <= L'9') || ch == L'.' || ch == L'-' || ch == L'_' || ch == L'+')
			{
				continue;
			}
			return false;
		}
		return true;
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

void ModManager::refresh()
{
	ensureModsDirectory();
	scanMods();
}

const std::vector<ModInfo>& ModManager::getMods() const
{
	return m_mods;
}

const std::vector<ModInfo::DataDefinition>& ModManager::getDataDefinitions() const
{
	return m_dataDefinitions;
}

const std::vector<ModManager::ScanDiagnostic>& ModManager::getScanDiagnostics() const
{
	return m_scanDiagnostics;
}

const File& ModManager::getModsDirectory() const
{
	return m_modsDirectory;
}

std::wstring ModManager::resolveFolderAssetOverride(const std::wstring& relativePath) const
{
	const std::wstring normalizedRelativePath = NormalizeAssetOverridePath(relativePath);
	if (normalizedRelativePath.empty())
	{
		return L"";
	}

	const std::wstring cacheKey = ToLowerCopy(normalizedRelativePath);
	const auto cacheIt = m_assetOverrideCache.find(cacheKey);
	if (cacheIt != m_assetOverrideCache.end())
	{
		return cacheIt->second;
	}

	static const std::wstring candidatePrefixes[] =
	{
		L"",
		L"textures",
		L"minecraft",
		// Use forward slashes here so the string literal cannot accidentally
		// introduce escape sequences like '\t' on Windows path segments.
		L"minecraft/textures"
	};

	for (auto it = m_mods.rbegin(); it != m_mods.rend(); ++it)
	{
		for (const std::wstring& assetRoot : it->assetRoots)
		{
			for (const std::wstring& prefix : candidatePrefixes)
			{
				const std::wstring relativeCandidate = prefix.empty() ? normalizedRelativePath : (prefix + L"/" + normalizedRelativePath);
				File candidate(assetRoot, relativeCandidate);
				if (candidate.exists() && candidate.isFile())
				{
					const std::wstring resolvedPath = candidate.getPath();
					m_assetOverrideCache[cacheKey] = resolvedPath;
					return resolvedPath;
				}
			}
		}
	}
	m_assetOverrideCache[cacheKey] = L"";
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
	m_dataDefinitions.clear();
	m_scanDiagnostics.clear();
	m_assetOverrideCache.clear();
	std::vector<File*>* files = m_modsDirectory.listFiles();
	if (files == nullptr)
	{
		return;
	}

	std::sort(files->begin(), files->end(), [](const File* lhs, const File* rhs)
	{
		if (lhs == nullptr || rhs == nullptr)
		{
			return lhs < rhs;
		}
		return ToLowerCopy(lhs->getName()) < ToLowerCopy(rhs->getName());
	});

	std::set<std::wstring> loadedModIds;

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
			const std::wstring normalizedId = ToLowerCopy(info.id);
			if (!normalizedId.empty() && loadedModIds.find(normalizedId) != loadedModIds.end())
			{
				app.DebugPrintf("ModManager: skipping %ls because mod id %ls is already loaded\n", info.sourcePath.c_str(), info.id.c_str());
				m_scanDiagnostics.push_back({ info.sourcePath, L"warning", L"Skipped because another mod with the same id was already loaded." });
				delete file;
				continue;
			}

			if (!normalizedId.empty())
			{
				loadedModIds.insert(normalizedId);
			}

			app.DebugPrintf("ModManager: found %ls (%ls) v%ls at %ls\n",
				info.getDisplayName().c_str(),
				info.getSourceTypeName().c_str(),
				info.version.empty() ? L"unknown" : info.version.c_str(),
				info.sourcePath.c_str());

			for (const std::wstring& warning : info.warnings)
			{
				app.DebugPrintf("ModManager: warning for %ls: %ls\n", info.getDisplayName().c_str(), warning.c_str());
			}
			for (const std::wstring& error : info.errors)
			{
				app.DebugPrintf("ModManager: error for %ls: %ls\n", info.getDisplayName().c_str(), error.c_str());
			}
			for (const ModInfo::DataDefinition& definition : info.dataDefinitions)
			{
				if (definition.valid)
				{
					app.DebugPrintf("ModManager: data definition %ls (%ls) loaded from %ls\n",
						definition.identifier.c_str(),
						definition.getTypeName().c_str(),
						definition.sourcePath.c_str());
				}
				else
				{
					app.DebugPrintf("ModManager: skipped data definition candidate %ls (%ls)\n",
						definition.sourcePath.c_str(),
						definition.validationMessage.c_str());
				}
			}

			for (const ModInfo::DataDefinition& definition : info.dataDefinitions)
			{
				if (definition.valid)
				{
					m_dataDefinitions.push_back(definition);
				}
			}
			m_mods.push_back(info);
		}
		else if (!info.sourcePath.empty())
		{
			if (info.errors.empty() && info.warnings.empty())
			{
				m_scanDiagnostics.push_back({ info.sourcePath, L"info", L"Skipped because it did not match a supported phase-1 mod layout." });
			}
			for (const std::wstring& warning : info.warnings)
			{
				m_scanDiagnostics.push_back({ info.sourcePath, L"warning", warning });
			}
			for (const std::wstring& error : info.errors)
			{
				m_scanDiagnostics.push_back({ info.sourcePath, L"error", error });
			}
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
	outInfo.runtimeAssetOverrides = true;
	outInfo.runtimeDataDiscovery = true;

	// Legacy root-first lookup keeps simple phase-1 folder mods working even before a full resource pack style mount exists.
	AddUniquePath(outInfo.assetRoots, path.getPath());

	const File manifest(path, kManifestFileName);
	const File assets(path, kAssetsDirectoryName);
	const File data(path, kDataDirectoryName);
	const File legacyTextures(path, L"textures");
	const File legacyMinecraft(path, L"minecraft");
	const bool hasRecognizedContent = manifest.exists() || assets.isDirectory() || data.isDirectory() || legacyTextures.isDirectory() || legacyMinecraft.isDirectory();
	if (!hasRecognizedContent)
	{
		app.DebugPrintf("ModManager: skipping folder %ls because it has no mod.json, assets/, data/, textures/, or minecraft/ content\n", path.getPath().c_str());
		outInfo.warnings.push_back(L"No supported content found (expected mod.json, assets/, data/, textures/, or minecraft/).");
		return false;
	}

	if (manifest.exists() && manifest.isFile())
	{
		const std::string manifestText = ReadFileToString(manifest);
		outInfo.manifestPath = manifest.getPath();
		outInfo.hasManifest = ParseManifest(manifestText, outInfo);
	}

	if (assets.exists() && assets.isDirectory())
	{
		AddUniquePath(outInfo.assetRoots, assets.getPath());
	}

	if (data.exists() && data.isDirectory())
	{
		AddUniquePath(outInfo.dataRoots, data.getPath());
	}

	if (!outInfo.dataRoots.empty() && !loadFolderDataDefinitions(path, outInfo))
	{
		outInfo.warnings.push_back(L"No valid data definitions were discovered under data/.");
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
	outInfo.runtimeAssetOverrides = false;
	outInfo.runtimeDataDiscovery = false;

	std::string manifestText;
	if (!SimpleZipReader::ReadTextFile(path.getPath(), kManifestFileName, manifestText))
	{
		app.DebugPrintf("ModManager: skipping zip %ls because no readable mod.json was found\n", path.getPath().c_str());
		outInfo.warnings.push_back(L"Zip was skipped because no readable mod.json was found.");
		return false;
	}

	outInfo.manifestPath = path.getPath() + L"!/mod.json";
	outInfo.hasManifest = ParseManifest(manifestText, outInfo);
	if (!outInfo.hasManifest)
	{
		outInfo.warnings.push_back(L"mod.json was readable but did not contain recognized metadata fields; using fallback values.");
	}

	// Phase 1 keeps folder mods as the only supported runtime asset override path.
	outInfo.warnings.push_back(L"Zip mods are metadata-only in the current implementation; folder mods remain the runtime-supported path.");
	return true;
}

bool ModManager::ParseManifest(const std::string& manifestText, ModInfo& outInfo)
{
	const std::wstring trimmedManifest = TrimAsciiWhitespace(manifestText);
	const std::wstring id = ExtractJsonString(manifestText, "id");
	const std::wstring name = ExtractJsonString(manifestText, "name");
	const std::wstring version = ExtractJsonString(manifestText, "version");
	const std::wstring description = ExtractJsonString(manifestText, "description");
	const std::wstring author = ExtractJsonString(manifestText, "author");
	const std::vector<std::wstring> authors = ExtractJsonStringArray(manifestText, "authors");

	if (trimmedManifest.empty())
	{
		outInfo.errors.push_back(L"mod.json was empty.");
		return false;
	}
	if (trimmedManifest.size() < 2 || trimmedManifest.front() != L'{' || trimmedManifest.back() != L'}')
	{
		outInfo.errors.push_back(L"mod.json must be a JSON object.");
		return false;
	}

	if (!id.empty())
	{
		std::wstring normalizedId;
		if (ValidateAndNormalizeModId(id, normalizedId))
		{
			outInfo.id = normalizedId;
		}
		else
		{
			outInfo.errors.push_back(L"mod.json field 'id' used unsupported characters; using a folder/file-name fallback id instead.");
		}
	}
	else
	{
		outInfo.warnings.push_back(L"mod.json is missing the recommended 'id' field; using a folder/file-name fallback id.");
	}
	if (!name.empty()) outInfo.name = name;
	else outInfo.warnings.push_back(L"mod.json is missing the recommended 'name' field; using a folder/file-name fallback name.");
	if (!version.empty()) outInfo.version = version;
	else outInfo.warnings.push_back(L"mod.json is missing the recommended 'version' field; using fallback version 0.0.0.");
	if (!description.empty()) outInfo.description = description;
	else outInfo.warnings.push_back(L"mod.json is missing the optional 'description' field.");
	if (!author.empty())
	{
		outInfo.author = author;
		AddUniqueString(outInfo.authors, author);
	}
	for (const std::wstring& authorName : authors)
	{
		AddUniqueString(outInfo.authors, authorName);
	}
	if (outInfo.authors.empty())
	{
		outInfo.warnings.push_back(L"mod.json does not define 'author' or 'authors'.");
	}
	if (!version.empty() && !LooksLikeSimpleVersion(version))
	{
		outInfo.warnings.push_back(L"mod.json field 'version' uses an unusual format; expected a simple dotted/semver-like value.");
	}

	return true;
}

bool ModManager::loadFolderDataDefinitions(const File& path, ModInfo& outInfo) const
{
	bool loadedAnyValidDefinition = false;
	for (const std::wstring& dataRootPath : outInfo.dataRoots)
	{
		File dataRoot(dataRootPath);
		if (!dataRoot.exists() || !dataRoot.isDirectory())
		{
			continue;
		}

		const size_t previousCount = outInfo.dataDefinitions.size();
		CollectDataDefinitionsRecursive(dataRoot, L"", outInfo);
		for (size_t i = previousCount; i < outInfo.dataDefinitions.size(); ++i)
		{
			if (outInfo.dataDefinitions[i].valid)
			{
				loadedAnyValidDefinition = true;
			}
		}
	}

	return loadedAnyValidDefinition;
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

std::vector<std::wstring> ModManager::ExtractJsonStringArray(const std::string& json, const std::string& key)
{
	std::vector<std::wstring> values;
	const std::string quotedKey = std::string("\"") + key + "\"";
	const size_t keyPos = json.find(quotedKey);
	if (keyPos == std::string::npos)
	{
		return values;
	}

	const size_t colonPos = json.find(':', keyPos + quotedKey.size());
	if (colonPos == std::string::npos)
	{
		return values;
	}

	size_t valueStart = colonPos + 1;
	while (valueStart < json.size() && isspace(static_cast<unsigned char>(json[valueStart])))
	{
		++valueStart;
	}
	if (valueStart >= json.size() || json[valueStart] != '[')
	{
		return values;
	}

	for (size_t i = valueStart + 1; i < json.size(); ++i)
	{
		while (i < json.size() && (isspace(static_cast<unsigned char>(json[i])) || json[i] == ','))
		{
			++i;
		}
		if (i >= json.size() || json[i] == ']')
		{
			break;
		}
		if (json[i] != '"')
		{
			break;
		}

		++i;
		std::string value;
		for (; i < json.size(); ++i)
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
		AddUniqueString(values, convStringToWstring(value));
	}

	return values;
}

void ModManager::CollectDataDefinitionsRecursive(const File& currentPath, const std::wstring& relativePath, ModInfo& outInfo)
{
	if (currentPath.isDirectory())
	{
		std::vector<File*>* files = currentPath.listFiles();
		if (files == nullptr)
		{
			return;
		}

		std::sort(files->begin(), files->end(), [](const File* lhs, const File* rhs)
		{
			if (lhs == nullptr || rhs == nullptr)
			{
				return lhs < rhs;
			}
			return ToLowerCopy(lhs->getName()) < ToLowerCopy(rhs->getName());
		});

		for (File* child : *files)
		{
			if (child == nullptr)
			{
				continue;
			}

			const std::wstring childRelativePath = relativePath.empty() ? child->getName() : (relativePath + L"/" + child->getName());
			CollectDataDefinitionsRecursive(*child, childRelativePath, outInfo);
			delete child;
		}

		delete files;
		return;
	}

	if (!currentPath.isFile() || !EndsWithIgnoreCase(currentPath.getName(), L".json"))
	{
		return;
	}

	ModInfo::DataDefinition definition;
	definition.sourcePath = currentPath.getPath();
	definition.pathId = NormalizePathSeparators(relativePath);

	const std::vector<std::wstring> segments = SplitPath(definition.pathId);
	if (segments.size() < 3)
	{
		definition.validationMessage = L"Data definitions must live under data/<namespace>/<category>/.../*.json.";
		outInfo.dataDefinitions.push_back(definition);
		outInfo.errors.push_back(definition.validationMessage + L" Source: " + definition.sourcePath);
		return;
	}

	definition.namespaceId = ToLowerCopy(segments[0]);
	if (!ValidateResourcePathSegment(definition.namespaceId))
	{
		definition.validationMessage = L"Data definition namespace contains unsupported characters.";
		outInfo.dataDefinitions.push_back(definition);
		outInfo.errors.push_back(definition.validationMessage + L" Source: " + definition.sourcePath);
		return;
	}

	const std::wstring category = ToLowerCopy(segments[1]);
	if (category == L"blocks") definition.type = ModInfo::DataDefinition::Type::Blocks;
	else if (category == L"items") definition.type = ModInfo::DataDefinition::Type::Items;
	else if (category == L"entities") definition.type = ModInfo::DataDefinition::Type::Entities;
	else if (category == L"mechanics") definition.type = ModInfo::DataDefinition::Type::Mechanics;
	else if (category == L"world_rules") definition.type = ModInfo::DataDefinition::Type::WorldRules;
	else
	{
		definition.validationMessage = L"Unsupported data category '" + category + L"'; currently supported: blocks, items, entities, mechanics, world_rules.";
		outInfo.dataDefinitions.push_back(definition);
		outInfo.warnings.push_back(definition.validationMessage + L" Source: " + definition.sourcePath);
		return;
	}

	const std::wstring logicalPath = RemoveJsonExtension(JoinPath(segments, 2, segments.size()));
	if (!ValidateDefinitionPath(logicalPath))
	{
		definition.validationMessage = L"Data definition path contains unsupported characters.";
		outInfo.dataDefinitions.push_back(definition);
		outInfo.errors.push_back(definition.validationMessage + L" Source: " + definition.sourcePath);
		return;
	}

	definition.identifier = definition.namespaceId + L":" + logicalPath;
	definition.declaredId = ExtractJsonString(ReadFileToString(currentPath), "id");
	if (!definition.declaredId.empty() && ToLowerCopy(definition.declaredId) != ToLowerCopy(definition.identifier))
	{
		outInfo.warnings.push_back(L"Data definition " + definition.sourcePath + L" declares id '" + definition.declaredId + L"' but its canonical id is '" + definition.identifier + L"'.");
	}

	if (!ReadJsonFile(currentPath, definition.jsonText))
	{
		definition.validationMessage = L"Data definition file could not be read as a small JSON object.";
		outInfo.dataDefinitions.push_back(definition);
		outInfo.errors.push_back(definition.validationMessage + L" Source: " + definition.sourcePath);
		return;
	}

	const std::wstring trimmedJson = TrimAsciiWhitespace(definition.jsonText);
	if (trimmedJson.size() < 2 || trimmedJson.front() != L'{' || trimmedJson.back() != L'}')
	{
		definition.validationMessage = L"Data definition must be a JSON object.";
		outInfo.dataDefinitions.push_back(definition);
		outInfo.errors.push_back(definition.validationMessage + L" Source: " + definition.sourcePath);
		return;
	}

	definition.valid = true;
	definition.validationMessage = L"ok";
	outInfo.dataDefinitions.push_back(definition);
}

bool ModManager::ReadJsonFile(const File& file, std::string& outText)
{
	const std::string text = ReadFileToString(file);
	if (text.empty() || text.size() > kMaxJsonFileBytes)
	{
		return false;
	}

	outText = text;
	return true;
}

bool ModManager::ValidateAndNormalizeModId(const std::wstring& input, std::wstring& outNormalized)
{
	if (input.empty())
	{
		return false;
	}

	outNormalized.clear();
	for (wchar_t ch : input)
	{
		const wchar_t lower = static_cast<wchar_t>(towlower(ch));
		if ((lower >= L'a' && lower <= L'z') || (lower >= L'0' && lower <= L'9') || lower == L'_' || lower == L'-' || lower == L'.')
		{
			outNormalized.push_back(lower);
		}
		else
		{
			return false;
		}
	}

	return !outNormalized.empty();
}

bool ModManager::ValidateResourcePathSegment(const std::wstring& input)
{
	if (input.empty())
	{
		return false;
	}

	for (wchar_t ch : input)
	{
		const wchar_t lower = static_cast<wchar_t>(towlower(ch));
		if ((lower >= L'a' && lower <= L'z') || (lower >= L'0' && lower <= L'9') || lower == L'_' || lower == L'-' || lower == L'.')
		{
			continue;
		}
		return false;
	}

	return true;
}

bool ModManager::ValidateDefinitionPath(const std::wstring& input)
{
	if (input.empty())
	{
		return false;
	}

	for (wchar_t ch : input)
	{
		const wchar_t lower = static_cast<wchar_t>(towlower(ch));
		if ((lower >= L'a' && lower <= L'z') || (lower >= L'0' && lower <= L'9') || lower == L'_' || lower == L'-' || lower == L'.' || lower == L'/')
		{
			continue;
		}
		return false;
	}

	return true;
}

std::wstring ModManager::NormalizePathSeparators(const std::wstring& input)
{
	std::wstring output = input;
	std::replace(output.begin(), output.end(), L'\\', L'/');
	return output;
}

std::wstring ModManager::NormalizeAssetOverridePath(const std::wstring& input)
{
	std::wstring output = NormalizePathSeparators(input);

	while (!output.empty() && (output.front() == L'/' || output.front() == L'\\'))
	{
		output.erase(output.begin());
	}

	if (output.size() > 4 && ToLowerCopy(output.substr(0, 4)) == L"res/")
	{
		output = output.substr(4);
	}
	if (output.size() > 17 && ToLowerCopy(output.substr(0, 17)) == L"assets/minecraft/")
	{
		output = output.substr(17);
	}
	else if (output.size() > 7 && ToLowerCopy(output.substr(0, 7)) == L"assets/")
	{
		output = output.substr(7);
	}
	if (output.size() > 10 && ToLowerCopy(output.substr(0, 10)) == L"minecraft/")
	{
		output = output.substr(10);
	}

	return output;
}

std::wstring ModManager::TrimAsciiWhitespace(const std::string& text)
{
	size_t start = 0;
	while (start < text.size() && isspace(static_cast<unsigned char>(text[start])))
	{
		++start;
	}

	size_t end = text.size();
	while (end > start && isspace(static_cast<unsigned char>(text[end - 1])))
	{
		--end;
	}

	return convStringToWstring(text.substr(start, end - start));
}

bool ModManager::EndsWithIgnoreCase(const std::wstring& value, const std::wstring& suffix)
{
	if (value.size() < suffix.size())
	{
		return false;
	}
	return toLower(value.substr(value.size() - suffix.size())) == toLower(suffix);
}
