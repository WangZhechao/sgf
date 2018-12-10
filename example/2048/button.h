#pragma once

#include <d2d1helper.h>
#include <d2d1.h>
#include "sgfd2drender.h"
#include "sgftexture.h"
#include "tileresource.h"

class Button
{
public:
	Button(ID2D1Bitmap* bitmap, const TextureRect& rect);
	virtual ~Button();

public:
	void setPostion(const TextureRect& renderRect);
	void draw(const std::shared_ptr<SGF::D2DRender>& d2dRender);
	void draw(const std::shared_ptr<SGF::D2DRender>& d2dRender, float opacity);
	bool hitTest(float x, float y);
	void press(bool);
	bool press() const { return _press; }

private:
	bool _press;
	ID2D1Bitmap* _buttonBitmap;
	TextureRect _bitmapRect;
	TextureRect _renderRect;
};