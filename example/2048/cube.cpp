#include "cube.h"
#include "tileresource.h"
#include <tchar.h>

const float CUBE_WIDTH = cubeRects[CT_0].size.width * CUBE_SCALE_SIZE;
const float CUBE_HEIGHT = cubeRects[CT_0].size.height * CUBE_SCALE_SIZE;

Cube::Cube()
	: _cubeBitmap(NULL), _currState(CS_STATIC), _currType(CT_0)
	, _row(0), _col(0), _scale(1.0f), _dt(0.08f)
{

}

Cube::~Cube()
{

}

void Cube::setPos(int row, int col)
{
	_row = row;
	_col = col;
}

void Cube::setBitmap(ID2D1Bitmap* bitmap)
{
	_cubeBitmap = bitmap;
}

const CUBE_TYPE Cube::type() const 
{
	return _currType;
}

void Cube::type(CUBE_TYPE t)
{
	_currType = t;
}

bool Cube::empty()
{
	if (_currType == CT_0)
		return true;

	return false;
}

bool Cube::isMerge(const Cube& c)
{
	if (_currType == c.type()
		&& _currType != CT_4096) {
		return true;
	}

	return false;
}

bool Cube::merge(Cube& c, size_t& score)
{
	score = 0;

	if (isMerge(c)) {
		c.type(CT_0);
		score = _currType * 5;
		_currState = CS_ANIMATIONS;
		_currType = (CUBE_TYPE)(_currType + 1);
		return true;
	}

	return false;
}

void Cube::swap(Cube& c)
{
	CUBE_TYPE t = c.type();
	c.type(_currType);
	_currType = t;
}

bool Cube::isWin()
{
	return (_currState == CS_STATIC && _currType == CT_4096);
}


void Cube::update()
{
	if (_currState == CS_ANIMATIONS) {
		_scale += _dt;

		if (_scale >= MAX_SCALE_VALUE) 
		{
			_dt = -_dt;
			_scale = MAX_SCALE_VALUE;
		}
		else if (_scale < 1.0f)
		{
			_dt = -_dt;
			_scale = 1.0f;
			_currState = CS_STATIC;
		}
	}
}


void Cube::draw(const std::shared_ptr<SGF::D2DRender>& d2dRender, float beginX, float beginY)
{
	const float dtH = cubeRects[_currType].size.height * CUBE_SCALE_SIZE - cubeRects[CT_0].size.height * CUBE_SCALE_SIZE;
	const float dtW = cubeRects[_currType].size.width * CUBE_SCALE_SIZE - cubeRects[CT_0].size.width * CUBE_SCALE_SIZE;
	const float x = beginX - dtW + CUBE_WIDTH * 0.5 * (_row + _col);
	const float y = beginY - dtH + (VIEW_BUILD_SIZE - 1) * CUBE_HEIGHT * 0.5f + (CUBE_HEIGHT * 0.5f - 16 * CUBE_SCALE_SIZE) * (_row - _col);
	const float w = cubeRects[_currType].size.width * CUBE_SCALE_SIZE;
	const float h = cubeRects[_currType].size.height * CUBE_SCALE_SIZE;


	if (_currState == CS_ANIMATIONS)
	{
		d2dRender->drawBitmap(x - (CUBE_SCALE_SIZE * (_scale - 1.0f)) * 0.5f * cubeRects[CT_0].size.width * CUBE_SCALE_SIZE,
			y - (CUBE_SCALE_SIZE * (_scale - 1.0f)) * 1.0f * cubeRects[CT_0].size.height * CUBE_SCALE_SIZE,
			w + (CUBE_SCALE_SIZE * (_scale - 1.0f)) * cubeRects[CT_0].size.width * CUBE_SCALE_SIZE,
			h + (CUBE_SCALE_SIZE * (_scale - 1.0f)) * cubeRects[CT_0].size.height * CUBE_SCALE_SIZE,
			_cubeBitmap, cubeRects[_currType].pos.x, cubeRects[_currType].pos.y, cubeRects[_currType].size.width, 
			cubeRects[_currType].size.height);
	}
	else if(_currState == CS_STATIC)
	{
		d2dRender->drawBitmap(x, y, w, h, _cubeBitmap, cubeRects[_currType].pos.x,
			cubeRects[_currType].pos.y, cubeRects[_currType].size.width, cubeRects[_currType].size.height);
	}
}


void Cube::drawDebugInfo(const std::shared_ptr<SGF::D2DRender>& d2dRender, IDWriteTextFormat * format, ID2D1SolidColorBrush * brush, float beginX, float beginY)
{
	ID2D1HwndRenderTarget* renderTarget = d2dRender->getHwndRenderTarget();

	TCHAR coordinate[32] = { 0 };
	TCHAR value[16] = { 0 };

	_stprintf(coordinate, TEXT("(%d,%d)"), _row, _col);
	_stprintf(value, TEXT("%d"), _currType == CT_0 ? 0 : (1 << _currType));

	float x = beginX + CUBE_WIDTH * 0.5f * (_row + _col);
	float y = beginY + (VIEW_BUILD_SIZE - 1) * CUBE_HEIGHT * 0.5f + (CUBE_HEIGHT * 0.5f - 16 * CUBE_SCALE_SIZE) * (_row - _col);
	format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	renderTarget->DrawText(coordinate, _tcsclen(coordinate), format, D2D1::RectF(x, y + 10, x + CUBE_WIDTH, y + CUBE_WIDTH), brush);

	brush->SetColor(D2D1::ColorF(D2D1::ColorF::Green));
	renderTarget->DrawText(value, _tcsclen(value), format, D2D1::RectF(x, y + 35, x + CUBE_WIDTH, y + CUBE_WIDTH), brush);
	format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
}


void Cube::clear()
{
	_currState = CS_STATIC;
	_currType = CT_0;
	_scale = 1.0f;
	_dt = 0.08f;
	_cubeBitmap = NULL;
	_row = 0;
	_col = 0;
}