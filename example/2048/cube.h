#pragma once

#include <d2d1helper.h>
#include <d2d1.h>
#include "sgfd2drender.h"

const float MAX_SCALE_VALUE = 1.2f;

enum CUBE_TYPE
{
	CT_0 = 0,
	CT_2,//5
	CT_4,//10
	CT_8,//15
	CT_16,
	CT_32,
	CT_64,
	CT_128,
	CT_256,
	CT_512,
	CT_1024,
	CT_2048,
	CT_4096
};


enum CUBE_STATE
{
	CS_STATIC = 0,
	CS_ANIMATIONS
};

namespace SGF
{
	class D2DRender;
}

const int VIEW_BUILD_SIZE = 4;
const int DEFAULT_DEBUG_FONT_WIDTH = 15;

class Cube
{
public:
	Cube();
	virtual ~Cube();

public:
	void setPos(int i, int j);
	void setBitmap(ID2D1Bitmap* bitmap);
	void update();
	void draw(const std::shared_ptr<SGF::D2DRender>& d2dRender, float beginX, float beginY);
	void drawDebugInfo(const std::shared_ptr<SGF::D2DRender>& d2dRender, 
		IDWriteTextFormat* format, ID2D1SolidColorBrush* brush, float beginX, 
		float beginY);
	bool empty();
	bool merge(Cube& c, size_t& score);
	bool isMerge(const Cube& c);
	void swap(Cube& c);
	const CUBE_TYPE type() const;
	void type(CUBE_TYPE t);
	bool isWin();
	void clear();

private:
	ID2D1Bitmap* _cubeBitmap;
	CUBE_STATE _currState;
	CUBE_TYPE _currType;
	int _row;
	int _col;
	float _scale;
	float _dt;
};