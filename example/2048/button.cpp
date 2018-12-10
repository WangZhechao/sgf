#include "button.h"
#include "sgfd2drender.h"
#include <tchar.h>


Button::Button(ID2D1Bitmap * bitmap, const TextureRect & rect)
	: _buttonBitmap(bitmap), _bitmapRect(rect), _renderRect(rect)
	, _press(false)
{
	
}

Button::~Button()
{

}


void Button::setPostion(const TextureRect& renderRect)
{
	_renderRect = renderRect;
}


void Button::draw(const std::shared_ptr<SGF::D2DRender>& d2dRender)
{
	d2dRender->drawBitmap(_renderRect.pos.x, _renderRect.pos.y, _renderRect.size.width,
		_renderRect.size.height, _buttonBitmap, _bitmapRect.pos.x,
		_bitmapRect.pos.y, _bitmapRect.size.width, _bitmapRect.size.height, _press ? 0.8f : 1.0f);
}

void Button::draw(const std::shared_ptr<SGF::D2DRender>& d2dRender, float opacity)
{
	d2dRender->drawBitmap(_renderRect.pos.x, _renderRect.pos.y, _renderRect.size.width,
		_renderRect.size.height, _buttonBitmap, _bitmapRect.pos.x,
		_bitmapRect.pos.y, _bitmapRect.size.width, _bitmapRect.size.height, opacity);
}

bool Button::hitTest(float x, float y)
{
	float px = abs(x - (_renderRect.pos.x + _renderRect.size.width * 0.5f));
	float py = abs(-y + (_renderRect.pos.y + _renderRect.size.height * 0.5f));

	if ((px * _renderRect.size.height * 0.5f + py * _renderRect.size.width * 0.5f) < 
		_renderRect.size.width * _renderRect.size.height * 0.25f)
	{
		return true;
	}

	return false;
}

void Button::press(bool p)
{
	_press = p;
}