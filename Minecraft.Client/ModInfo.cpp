#include "stdafx.h"
#include "ModInfo.h"

std::wstring ModInfo::getDisplayName() const
{
	if (!name.empty())
	{
		return name;
	}
	return id;
}

std::wstring ModInfo::getSourceTypeName() const
{
	return sourceType == SourceType::ZipArchive ? L"zip" : L"folder";
}
