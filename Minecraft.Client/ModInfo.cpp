#include "stdafx.h"
#include "ModInfo.h"

std::wstring ModInfo::DataDefinition::getTypeName() const
{
	switch (type)
	{
	case Type::Blocks: return L"blocks";
	case Type::Items: return L"items";
	case Type::Entities: return L"entities";
	case Type::Mechanics: return L"mechanics";
	case Type::WorldRules: return L"world_rules";
	default: return L"unknown";
	}
}

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

bool ModInfo::hasIssues() const
{
	return !warnings.empty() || !errors.empty();
}
