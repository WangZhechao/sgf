#pragma once

#include "sgfgameapp.h"
#include "sgfbitmappixels.h"
#include <vector>
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>


class WaterWave : public SGF::GameApp
{
public:
	WaterWave(HINSTANCE);
	~WaterWave();


public:
	virtual int gameInitiate();
	virtual void gameUpdate(float dt);
	virtual void gameRender(float lag);
	virtual void gameUninitiate();

private:
	void rippleSpread();
	void dropStone(int x, int y, int r, int h);
	void renderRipple();

private:
	SGF::BitmapPixels _bgPixels;
	ID2D1Bitmap* _bgBitmap;
	unsigned char* _waterPixels;
	int* _amplitude;
	int _curPos;
	SIZE _win;
};