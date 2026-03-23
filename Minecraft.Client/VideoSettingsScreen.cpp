#include "stdafx.h"
#include "VideoSettingsScreen.h"
#include "SmallButton.h"
#include "SlideButton.h"
#include "Options.h"
#include "ControlsScreen.h"
#include "..\\Minecraft.World\\net.minecraft.locale.h"

namespace
{
	constexpr int kButtonDone = 200;
	constexpr int kButtonWindowedResolution = 201;
}

VideoSettingsScreen::VideoSettingsScreen(Screen *lastScreen, Options *options)
{
	this->title = L"Video Settings"; // 4J - added
    this->lastScreen = lastScreen;
    this->options = options;
}

void VideoSettingsScreen::init()
{
    Language *language = Language::getInstance();
    this->title = language->getElement(L"options.videoTitle");

	const Options::Option *items[] = {
		Options::Option::GRAPHICS,
		Options::Option::RENDER_DISTANCE,
		Options::Option::FOV,
		Options::Option::GAMMA,
		Options::Option::AMBIENT_OCCLUSION,
		Options::Option::RENDER_CLOUDS,
		Options::Option::VIEW_BOBBING,
		Options::Option::PARTICLES,
		Options::Option::GUI_SCALE,
		Options::Option::FRAMERATE_LIMIT,
		Options::Option::SHOW_FPS,
		Options::Option::ADVANCED_OPENGL
	};

	const int itemCount = static_cast<int>(sizeof(items) / sizeof(items[0]));
	for (int position = 0; position < itemCount; position++)
	{
		const Options::Option *item = items[position];
        if (!item->isProgress())
		{
            buttons.push_back(new SmallButton(
                item->getId(),
                width / 2 - 155 + position % 2 * 160,
                height / 6 + 24 * (position >> 1),
                item,
                options->getMessage(item)
            ));
        }
		else
		{
            buttons.push_back(new SlideButton(
                item->getId(),
                width / 2 - 155 + position % 2 * 160,
                height / 6 + 24 * (position >> 1),
                item,
                options->getMessage(item),
                options->getProgressValue(item)
            ));
        }
    }

#ifdef _WINDOWS64
	buttons.push_back(new Button(
		kButtonWindowedResolution,
		width / 2 - 100,
		height / 6 + 24 * ((itemCount + 1) >> 1) + 12,
		options->getWindowedResolutionMessage()
	));
	buttons.push_back(new Button(
		kButtonDone,
		width / 2 - 100,
		height / 6 + 24 * ((itemCount + 3) >> 1) + 16,
		language->getElement(L"gui.done")
	));
#else
	buttons.push_back(new Button(
		kButtonDone,
		width / 2 - 100,
		height / 6 + 24 * ((itemCount + 1) >> 1) + 12,
		language->getElement(L"gui.done")
	));
#endif
}

void VideoSettingsScreen::buttonClicked(Button *button)
{
    if (!button->active) return;

    if (button->id < 100 && (dynamic_cast<SmallButton *>(button) != nullptr))
	{
        options->toggle(static_cast<SmallButton *>(button)->getOption(), 1);
        button->msg = options->getMessage(Options::Option::getItem(button->id));
    }

	if (button->id == kButtonWindowedResolution)
	{
		options->cycleWindowedResolution(1);
		button->msg = options->getWindowedResolutionMessage();
		minecraft->options->save();
	}

    if (button->id == kButtonDone)
	{
        minecraft->options->save();
        minecraft->setScreen(lastScreen);
    }

    ScreenSizeCalculator ssc(minecraft->options, minecraft->width, minecraft->height);
    int screenWidth = ssc.getWidth();
    int screenHeight = ssc.getHeight();
    Screen::init(minecraft, screenWidth, screenHeight);
}

void VideoSettingsScreen::render(int xm, int ym, float a)
{
    renderBackground();
    drawCenteredString(font, title, width / 2, 20, 0xffffff);
    Screen::render(xm, ym, a);
}
