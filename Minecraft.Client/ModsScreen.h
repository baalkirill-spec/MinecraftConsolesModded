#pragma once

#include "Screen.h"

class ModsScreen : public Screen
{
private:
	Screen* lastScreen;
	int page;

public:
	explicit ModsScreen(Screen* lastScreen);
	virtual void init();

protected:
	virtual void buttonClicked(Button* button);

public:
	virtual void render(int xm, int ym, float a);
};
