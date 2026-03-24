#pragma once

#include "Screen.h"

class Button;
class EditBox;

class LauncherScreen : public Screen
{
private:
	EditBox *playerNameEdit;
	std::wstring statusMessage;
	std::wstring detailMessage;
	Button *startButton;

public:
	LauncherScreen();
	virtual void tick();
	virtual void init();
	virtual void removed();

protected:
	virtual void keyPressed(wchar_t ch, int eventKey);
	virtual void buttonClicked(Button *button);
	virtual void mouseClicked(int x, int y, int buttonNum);

public:
	virtual void render(int xm, int ym, float a);

private:
	void validateNameAndRefreshUi();
	void startGame();
};
