#include "stdafx.h"
#include "ModManager.h"

ModManager::ModManager()
{
}

void ModManager::init(const File& workingDirectory)
{
	m_workingDirectory = workingDirectory;
	m_modsDirectory = File(workingDirectory, L"mods");
	ensureModsDirectory();
	reload();
}

void ModManager::reload()
{
	m_mods.clear();
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

void ModManager::ensureModsDirectory()
{
	if (!m_modsDirectory.exists())
	{
		m_modsDirectory.mkdirs();
	}
}

void ModManager::scanMods()
{
	if (!m_modsDirectory.exists() || !m_modsDirectory.isDirectory())
	{
		return;
	}

	std::vector<File*>* files = m_modsDirectory.listFiles();
	if (files == nullptr)
	{
		return;
	}

	for (size_t i = 0; i < files->size(); ++i)
	{
		File* file = (*files)[i];
		if (file == nullptr)
		{
			continue;
		}

		if (file->isDirectory())
		{
			addFolderMod(*file);
		}
		else if (file->isFile() && hasExtension(file->getName(), L".zip"))
		{
			addZipMod(*file);
		}
	}
}

void ModManager::addFolderMod(const File& file)
{
	ModInfo info;
	info.id = file.getName();
	info.name = file.getName();
	info.version = L"folder";
	info.source = file;
	info.isZip = false;
	info.enabled = true;
	m_mods.push_back(info);
}

void ModManager::addZipMod(const File& file)
{
	ModInfo info;
	info.id = file.getName();
	info.name = file.getName();
	info.version = L"zip";
	info.source = file;
	info.isZip = true;
	info.enabled = true;
	m_mods.push_back(info);
}

bool ModManager::hasExtension(const std::wstring& fileName, const std::wstring& extension) const
{
	if (fileName.length() < extension.length())
	{
		return false;
	}

	const size_t offset = fileName.length() - extension.length();
	return fileName.substr(offset, extension.length()) == extension;
}
