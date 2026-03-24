#include "stdafx.h"
#include "LauncherScreen.h"

#include "Button.h"
#include "EditBox.h"
#include "Minecraft.h"
#include "Options.h"
#include "TitleScreen.h"
#include "Textures.h"
#include "User.h"

#include "..\\Minecraft.World\\StringHelpers.h"
#include "..\\Minecraft.World\\System.h"

namespace
{
	enum LauncherButtonId
	{
		BUTTON_START = 0,
		BUTTON_QUIT = 1,
	};
}

LauncherScreen::LauncherScreen()
	: playerNameEdit(nullptr),
	  statusMessage(L""),
	  detailMessage(L""),
	  startButton(nullptr)
{
}

void LauncherScreen::tick()
{
	if (playerNameEdit != nullptr)
	{
		playerNameEdit->tick();
	}
}

void LauncherScreen::init()
{
	buttons.clear();

	const int editWidth = 220;
	const int editHeight = 20;
	const int editX = width / 2 - editWidth / 2;
	const int editY = height / 2 - 30;

	std::wstring initialName = L"Player";
	if (minecraft != nullptr && minecraft->options != nullptr)
	{
		initialName = Options::NormalizePlayerName(minecraft->options->playerName);
	}

	playerNameEdit = new EditBox(this, font, editX, editY, editWidth, editHeight, initialName);
	playerNameEdit->setMaxLength(16);
	playerNameEdit->focus(true);
	playerNameEdit->active = true;

	const int buttonWidth = 106;
	const int gap = 8;
	const int buttonY = editY + 34;
	buttons.push_back(startButton = new Button(BUTTON_START, width / 2 - buttonWidth - gap / 2, buttonY, buttonWidth, 20, L"Start Game"));
	buttons.push_back(new Button(BUTTON_QUIT, width / 2 + gap / 2, buttonY, buttonWidth, 20, L"Quit"));

	validateNameAndRefreshUi();
}

void LauncherScreen::removed()
{
	if (playerNameEdit != nullptr)
	{
		delete playerNameEdit;
		playerNameEdit = nullptr;
	}
}

void LauncherScreen::keyPressed(wchar_t ch, int eventKey)
{
	if (eventKey == Keyboard::KEY_RETURN)
	{
		startGame();
		return;
	}

	if (playerNameEdit != nullptr)
	{
		playerNameEdit->keyPressed(ch, eventKey);
	}

	validateNameAndRefreshUi();
}

void LauncherScreen::buttonClicked(Button *button)
{
	if (button->id == BUTTON_START)
	{
		startGame();
		return;
	}

	if (button->id == BUTTON_QUIT)
	{
		minecraft->stop();
	}
}

void LauncherScreen::mouseClicked(int x, int y, int buttonNum)
{
	Screen::mouseClicked(x, y, buttonNum);
	if (playerNameEdit != nullptr)
	{
		playerNameEdit->mouseClicked(x, y, buttonNum);
	}
}

void LauncherScreen::render(int xm, int ym, float a)
{
	renderBackground(static_cast<int>((System::currentTimeMillis() / 80) & 31));

	const int panelWidth = 300;
	const int panelLeft = width / 2 - panelWidth / 2;
	const int panelTop = height / 2 - 90;
	fill(panelLeft, panelTop, panelLeft + panelWidth, panelTop + 170, 0x88000000);
	fill(panelLeft, panelTop, panelLeft + panelWidth, panelTop + 1, 0xff606060);
	fill(panelLeft, panelTop + 169, panelLeft + panelWidth, panelTop + 170, 0xff202020);

	drawCenteredString(font, L"MINECRAFT", width / 2 + 1, panelTop + 12 + 1, 0x202020);
	drawCenteredString(font, L"MINECRAFT", width / 2, panelTop + 12, 0xffffff);
	drawCenteredString(font, L"Launcher", width / 2, panelTop + 26, 0xffe080);

	drawString(font, L"Player Name", width / 2 - 110, height / 2 - 42, 0xe0e0e0);
	if (playerNameEdit != nullptr)
	{
		playerNameEdit->render();
	}

	drawCenteredString(font, statusMessage, width / 2, height / 2 + 30, 0x90ff90);
	drawCenteredString(font, detailMessage, width / 2, height / 2 + 42, 0xb0b0b0);
	drawCenteredString(font, L"Allowed: A-Z a-z 0-9 _ (3-16 chars)", width / 2, height - 26, 0xa0a0a0);

	Screen::render(xm, ym, a);
}

void LauncherScreen::validateNameAndRefreshUi()
{
	const std::wstring trimmed = trimString(playerNameEdit != nullptr ? playerNameEdit->getValue() : L"");
	const bool valid = Options::IsValidPlayerName(trimmed);
	if (startButton != nullptr)
	{
		startButton->active = valid;
	}

	if (trimmed.empty())
	{
		statusMessage = L"Enter a player name to continue";
		detailMessage = L"";
		return;
	}

	if (valid)
	{
		statusMessage = L"Ready";
		detailMessage = L"";
	}
	else
	{
		statusMessage = L"Invalid player name";
		detailMessage = L"Use 3-16 letters, numbers, or underscore";
	}
}

void LauncherScreen::startGame()
{
	if (minecraft == nullptr || minecraft->options == nullptr || playerNameEdit == nullptr)
	{
		return;
	}

	const std::wstring trimmed = trimString(playerNameEdit->getValue());
	if (!Options::IsValidPlayerName(trimmed))
	{
		validateNameAndRefreshUi();
		return;
	}

	minecraft->options->playerName = trimmed;
	minecraft->options->save();
	if (minecraft->user == nullptr)
	{
		minecraft->user = new User(trimmed, L"");
	}
	minecraft->user->name = trimmed;
	minecraft->setScreen(new TitleScreen());
}
