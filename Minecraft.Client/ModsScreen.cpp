#include "stdafx.h"
#include "ModsScreen.h"
#include "Button.h"
#include "Minecraft.h"
#include "ModInfo.h"
#include "ModManager.h"
#include "Textures.h"
#include "..\\Minecraft.World\\net.minecraft.locale.h"

namespace
{
	constexpr int kModsPerPage = 6;
	constexpr int kButtonDone = 200;
	constexpr int kButtonRefresh = 201;
	constexpr int kButtonPrev = 202;
	constexpr int kButtonNext = 203;
}

ModsScreen::ModsScreen(Screen* lastScreen)
	: lastScreen(lastScreen),
	  page(0)
{
}

void ModsScreen::init()
{
	Language* language = Language::getInstance();
	buttons.push_back(new Button(kButtonDone, width / 2 - 100, height - 28, language->getElement(L"gui.done")));
	buttons.push_back(new Button(kButtonRefresh, width / 2 - 100, height - 52, 98, 20, L"Refresh Mods"));
	buttons.push_back(new Button(kButtonPrev, width / 2 - 100, height - 76, 48, 20, L"<"));
	buttons.push_back(new Button(kButtonNext, width / 2 + 52, height - 76, 48, 20, L">"));
}

void ModsScreen::buttonClicked(Button* button)
{
	if (button == nullptr || !button->active)
	{
		return;
	}

	if (button->id == kButtonDone)
	{
		minecraft->setScreen(lastScreen);
		return;
	}

	if (button->id == kButtonRefresh)
	{
		if (minecraft->modManager != nullptr)
		{
			minecraft->modManager->refresh();
			if (minecraft->textures != nullptr)
			{
				minecraft->textures->reloadAll();
			}
		}
		return;
	}

	if (minecraft->modManager == nullptr)
	{
		return;
	}

	const int modCount = static_cast<int>(minecraft->modManager->getMods().size());
	const int pageCount = std::max(1, (modCount + kModsPerPage - 1) / kModsPerPage);

	if (button->id == kButtonPrev)
	{
		page = (page + pageCount - 1) % pageCount;
	}
	else if (button->id == kButtonNext)
	{
		page = (page + 1) % pageCount;
	}
}

void ModsScreen::render(int xm, int ym, float a)
{
	renderBackground();

	drawCenteredString(font, L"Mods", width / 2, 16, 0xffffff);

	int y = 40;
	int x = width / 2 - 150;

	if (minecraft->modManager == nullptr)
	{
		drawCenteredString(font, L"Mod manager is not available.", width / 2, y, 0xff8080);
		Screen::render(xm, ym, a);
		return;
	}

	const std::vector<ModInfo>& mods = minecraft->modManager->getMods();
	const std::vector<ModInfo::DataDefinition>& definitions = minecraft->modManager->getDataDefinitions();
	const std::vector<ModManager::ScanDiagnostic>& diagnostics = minecraft->modManager->getScanDiagnostics();
	const int modCount = static_cast<int>(mods.size());
	const int pageCount = std::max(1, (modCount + kModsPerPage - 1) / kModsPerPage);
	if (page >= pageCount)
	{
		page = pageCount - 1;
	}

	drawString(font, L"Folder: " + minecraft->modManager->getModsDirectory().getPath(), 12, y, 0xe0e0e0);
	y += 12;
	drawString(font,
		L"Loaded mods: " + std::to_wstring(modCount) + L" | data defs: " + std::to_wstring(definitions.size()) +
		L" | page " + std::to_wstring(page + 1) + L"/" + std::to_wstring(pageCount),
		12,
		y,
		0xa0ffa0);
	y += 18;
	if (!diagnostics.empty())
	{
		drawString(font, L"Last scan diagnostics: " + std::to_wstring(diagnostics.size()), 12, y, 0xffc080);
		y += 12;
		const int maxVisible = std::min(3, static_cast<int>(diagnostics.size()));
		for (int i = 0; i < maxVisible; ++i)
		{
			const ModManager::ScanDiagnostic& diagnostic = diagnostics[i];
			const std::wstring line = L"- [" + diagnostic.severity + L"] " + diagnostic.message;
			drawString(font, line, 18, y, diagnostic.severity == L"error" ? 0xff8080 : 0xe0e0e0);
			y += 12;
		}
		y += 6;
	}

	if (mods.empty())
	{
		drawString(font, L"No folder or zip mods were discovered in the mods directory.", 12, y, 0xffd080);
		y += 12;
		drawString(font, L"Supported phase-1 layouts: mod.json, assets/, data/, textures/, minecraft/.", 12, y, 0xe0e0e0);
	}
	else
	{
		const int startIndex = page * kModsPerPage;
		const int endIndex = std::min(startIndex + kModsPerPage, modCount);
		for (int i = startIndex; i < endIndex; ++i)
		{
			const ModInfo& mod = mods[i];
			fillGradient(x - 8, y - 2, width / 2 + 150, y + 28, 0x60202020, 0x80303030);
			drawString(font, mod.getDisplayName() + L" [" + mod.getSourceTypeName() + L"]", x, y, 0xffffff);
			drawString(font, L"id: " + mod.id + L"  version: " + (mod.version.empty() ? L"unknown" : mod.version), x, y + 10, 0xd0d0d0);

			std::wstring status = L"assets: " + std::to_wstring(mod.assetRoots.size()) + L", data: " + std::to_wstring(mod.dataDefinitions.size());
			if (mod.hasIssues())
			{
				status += L", warnings: " + std::to_wstring(mod.warnings.size()) + L", errors: " + std::to_wstring(mod.errors.size());
			}
			drawString(font, status, x, y + 20, mod.hasIssues() ? 0xffc080 : 0xa0ffa0);
			y += 36;
		}
	}

	Screen::render(xm, ym, a);
}
