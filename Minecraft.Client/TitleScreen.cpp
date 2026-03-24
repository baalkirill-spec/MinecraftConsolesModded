#include "stdafx.h"
#include "Button.h"
#include "ClientConstants.h"
#include "Minecraft.h"
#include "OptionsScreen.h"
#include "ModsScreen.h"
#include "ModManager.h"
#include "SelectWorldScreen.h"
#include "JoinMultiplayerScreen.h"
#include "Tesselator.h"
#include "Textures.h"
#include "..\Minecraft.World\StringHelpers.h"
#include "..\Minecraft.World\InputOutputStream.h"
#include "..\Minecraft.World\net.minecraft.locale.h"
#include "..\Minecraft.World\System.h"
#include "..\Minecraft.World\Random.h"
#include "TitleScreen.h"

Random *TitleScreen::random = new Random();

TitleScreen::TitleScreen()
{
	// 4J - added initialisers
	vo = 0;
	multiplayerButton = nullptr;

    splash = L"";
//    try {	// 4J - removed try/catch
    vector<wstring> splashes;

	/*
    BufferedReader *br = new BufferedReader(new InputStreamReader(InputStream::getResourceAsStream(L"res\\title\\splashes.txt"))); //, Charset.forName("UTF-8")
		
    wstring line = L"";
    while ( !(line = br->readLine()).empty() )
	{
        line = trimString( line );
        if (line.length() > 0)
		{
            splashes.push_back(line);
        }
    }
	
    br->close();
	delete br;
	*/

    splash = L""; //splashes.at(random->nextInt(splashes.size()));

//    } catch (Exception e) {
//    }
}

void TitleScreen::tick()
{
	//vo += 1.0f;
	//if( vo > 100.0f ) minecraft->setScreen(new SelectWorldScreen(this));		// 4J - temp testing
}

void TitleScreen::keyPressed(wchar_t eventCharacter, int eventKey)
{
}

void TitleScreen::init()
{
	/* 4J - Implemented in main menu instead
    Calendar c = Calendar.getInstance();
    c.setTime(new Date());

    if (c.get(Calendar.MONTH) + 1 == 11 && c.get(Calendar.DAY_OF_MONTH) == 9) {
        splash = "Happy birthday, ez!";
    } else if (c.get(Calendar.MONTH) + 1 == 6 && c.get(Calendar.DAY_OF_MONTH) == 1) {
        splash = "Happy birthday, Notch!";
    } else if (c.get(Calendar.MONTH) + 1 == 12 && c.get(Calendar.DAY_OF_MONTH) == 24) {
        splash = "Merry X-mas!";
    } else if (c.get(Calendar.MONTH) + 1 == 1 && c.get(Calendar.DAY_OF_MONTH) == 1) {
        splash = "Happy new year!";
    }
	*/

    Language *language = Language::getInstance();

	const int buttonWidth = 200;
	const int smallButtonWidth = 98;
	const int spacing = 24;
	const int topPos = height / 4 + 46;

	buttons.push_back(new Button(1, width / 2 - buttonWidth / 2, topPos, buttonWidth, 20, language->getElement(L"menu.singleplayer")));
	buttons.push_back(multiplayerButton = new Button(2, width / 2 - buttonWidth / 2, topPos + spacing, buttonWidth, 20, language->getElement(L"menu.multiplayer")));
	buttons.push_back(new Button(0, width / 2 - buttonWidth / 2, topPos + spacing * 2, smallButtonWidth, 20, language->getElement(L"menu.options")));
	buttons.push_back(new Button(4, width / 2 + 2, topPos + spacing * 2, smallButtonWidth, 20, language->getElement(L"menu.quit")));
	buttons.push_back(new Button(5, width / 2 - buttonWidth / 2, topPos + spacing * 3, buttonWidth, 20, L"Mods"));

	if (minecraft->modManager != nullptr)
	{
		const int modCount = static_cast<int>(minecraft->modManager->getMods().size());
		if (modCount > 0)
		{
			splash = std::to_wstring(modCount) + L" mods loaded";
		}
		else
		{
			splash = L"Drop folder mods into ./mods";
		}
	}

    if (minecraft->user == nullptr)
	{
        multiplayerButton->active = false;
    }

}

void TitleScreen::buttonClicked(Button *button)
{
    if (button->id == 0)
	{
        minecraft->setScreen(new OptionsScreen(this, minecraft->options));
    }
    if (button->id == 1)
	{
        minecraft->setScreen(new SelectWorldScreen(this));
    }
    if (button->id == 2)
	{
        minecraft->setScreen(new JoinMultiplayerScreen(this));
    }
    if (button->id == 3)
	{
 //       minecraft->setScreen(new TexturePackSelectScreen(this));		// 4J - TODO put back in
    }
    if (button->id == 4)
	{
        minecraft->stop();
    }
	if (button->id == 5)
	{
		minecraft->setScreen(new ModsScreen(this));
	}
}

void TitleScreen::render(int xm, int ym, float a)
{
	renderBackground(static_cast<int>((System::currentTimeMillis() / 64) & 31));

	const int logoY = 20;
	const int panelWidth = 240;
	const int panelTop = height / 4 + 36;
	const int panelLeft = width / 2 - panelWidth / 2;
	fill(panelLeft, panelTop, panelLeft + panelWidth, panelTop + 104, 0x66000000);

	drawCenteredString(font, L"MINECRAFT", width / 2 + 2, logoY + 2, 0x202020);
	drawCenteredString(font, L"MINECRAFT", width / 2, logoY, 0xffffff);
	drawCenteredString(font, L"CONSOLES MODDED", width / 2 + 1, logoY + 13, 0x303000);
	drawCenteredString(font, L"CONSOLES MODDED", width / 2, logoY + 12, 0xffff55);

	if (!splash.empty())
	{
		drawCenteredString(font, splash, width / 2, logoY + 30, 0xffe080);
	}

	drawString(font, ClientConstants::VERSION_STRING, 6, 6, 0xb0b0b0);
	drawString(font, ClientConstants::BRANCH_STRING, 6, 16, 0x909090);
	drawString(font, L"Player: " + (minecraft != nullptr && minecraft->user != nullptr ? minecraft->user->name : L"Unknown"), 6, 26, 0xc0c0c0);

	drawCenteredString(font, L"Main Menu inspired by Java-era layout", width / 2, height - 42, 0xe0e0e0);
	drawCenteredString(font, L"Phase-1 modding + skins + FPS overlay preserved", width / 2, height - 30, 0xa0a0a0);

	Screen::render(xm, ym, a);
}
