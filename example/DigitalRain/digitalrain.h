#pragma once

#include "sgfgameapp.h"
#include <vector>
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>

class StrLine;

class DigitalRain : public SGF::GameApp
{
public:
	DigitalRain(HINSTANCE h);
	~DigitalRain();

public:
	virtual int gameInitiate();
	virtual void gameUpdate(float dt);
	virtual void gameRender(float lag);
	virtual void gameUninitiate();


private:
	int createFont();
	void destoryFont();
	void genCharLines();


private:
	SIZE _font;
	SIZE _win;
	std::vector<StrLine> _numLines;
	IDWriteTextFormat* _writeformat;
	ID2D1SolidColorBrush* _writeBrush;
};