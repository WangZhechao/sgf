#include "waterwave.h"
#include "sgfbasewindow.h"
#include "sgfd2drender.h"
#include "sgfeventdispatcher.h"
#include "sgfeventlistenermouse.h"
#include "sgfeventmouse.h"

#include <assert.h>
#include <math.h>


#pragma comment(lib, "dwrite")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

#define RAND_RANAGE(a, b) (a + (rand()%(b-a+1)))


WaterWave::WaterWave(HINSTANCE h)
	: SGF::GameApp(h), _bgBitmap(NULL), _waterPixels(NULL), _amplitude(NULL)
	, _curPos(0)
{
	_win = { 800, 600 };
	srand((int)GetTickCount()); //初始化随机数发生器
}

WaterWave::~WaterWave()
{

}

int WaterWave::gameInitiate()
{
	_d2dRender->getSize(_win);
	_baseWindow->setGameTitle(TEXT("水波"));

	//加载图片像素
	int ret = _d2dRender->loadBitmapFromFile(TEXT("bg.bmp"), _win.cx, _win.cy, &_bgPixels);
	if (ret != 0) {
		return -1;
	}


	assert(_bgPixels.props.width == _win.cx && _win.cy == _bgPixels.props.height);


	//创建背景位图
	ret = _d2dRender->createBitmap(_bgPixels.props.width, 
		_bgPixels.props.height, _bgPixels.data, &_bgPixels.props, &_bgBitmap);
	if (ret != 0) {
		return -2;
	}


	_waterPixels = new (std::nothrow) unsigned char[_bgPixels.size];
	if (_waterPixels) {
		memset(_waterPixels, 0, _bgPixels.size);
	}
	

	_amplitude = new (std::nothrow) int[2 * _win.cx * _win.cy];
	if (_amplitude) {
		memset(_amplitude, 0, 2 * _win.cx * _win.cy * sizeof(int));
	}


	//鼠标监听
	auto mouseListener = SGF::EventListenerMouse::create();
	mouseListener->onMouseMove = [this](SGF::EventMouse* event) {
		dropStone(event->getCursorX(), event->getCursorY(),
			RAND_RANAGE(5, 10), RAND_RANAGE(32, 150));
	};
	_eventDispatcher->addEventListener(mouseListener);

	return 0;
}


void WaterWave::gameUpdate(float dt)
{
	rippleSpread();
}


void WaterWave::gameRender(float lag)
{
	renderRipple();

	_d2dRender->copyMemToBmp(_bgBitmap, RECT({ 0, 0,
		(LONG)_bgPixels.props.width, (LONG)_bgPixels.props.height }),
		_waterPixels, _bgPixels.props.width * 4);

	_d2dRender->drawBitmap(0, 0, _bgBitmap);
}


void WaterWave::gameUninitiate()
{
	SGF::D2DSafeRelease(&_bgBitmap);

	if (_amplitude) {
		delete[] _amplitude;
		_amplitude = NULL;
	}

	if (_waterPixels) {
		delete[] _waterPixels;
		_waterPixels = NULL;
	}
}


void WaterWave::rippleSpread()
{
	int nextPost = (_curPos + 1) % 2;

	for (int i = 1; i < _win.cy - 1; i++)
	{
		for (int j = 1; j < _win.cx - 1; j++)
		{
			//振幅计算
			_amplitude[nextPost * _win.cx * _win.cy + _win.cx * i + j] = 
				((_amplitude[_curPos * _win.cx * _win.cy + _win.cx * (i-1) + j]
					+ _amplitude[_curPos * _win.cx * _win.cy + _win.cx * (i + 1) + j]
					+ _amplitude[_curPos * _win.cx * _win.cy + _win.cx * i + j - 1]
					+ _amplitude[_curPos * _win.cx * _win.cy + _win.cx * i + j + 1]) >> 1)
				- _amplitude[nextPost * _win.cx * _win.cy + _win.cx * i + j];

			
			//阻力
			_amplitude[nextPost * _win.cx * _win.cy + _win.cx * i + j] -= _amplitude[nextPost * _win.cx * _win.cy + _win.cx * i + j] >> 5;
		}
	}

	_curPos = (_curPos + 1) % 2;
}


//设置起始波动位置
void WaterWave::dropStone(int x, int y, int r, int h)
{
	POINT ptStart;
	SIZE szLen;

	ptStart.x = x - r < 0 ? 0 : x - r;
	ptStart.y = y - r < 0 ? 0 : y - r;
	szLen.cx = x + r >= _win.cx ? _win.cx - 1 : x + r;
	szLen.cy = y + r >= _win.cy ? _win.cy - 1 : y + r;

	for (int i = ptStart.y; i < szLen.cy; i++)
	{
		for (int j = ptStart.x; j < szLen.cx; j++)
		{
			if (((i - y)*(i - y) + (j - x)*(j - x)) < r*r)
				(_amplitude)[_curPos * _win.cx * _win.cy + _win.cx * i + j] = -h;
		}
	}
}


void WaterWave::renderRipple()
{
	int offsetX = 0;
	int offsetY = 0;
	int *pBitmap[2] = { (int*)_bgPixels.data, (int*)_waterPixels };
	int width = (int)_bgPixels.props.width;
	int height = (int)_bgPixels.props.height;


	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			//计算偏移
			offsetX = (_amplitude)[_curPos * width * height + width * i + j - 1] - (_amplitude)[_curPos * width * height + width * i + j + 1];
			offsetY = (_amplitude)[_curPos * width * height + width * (i - 1) + j] - (_amplitude)[_curPos * width * height + width * (i + 1) + j];

			//越界
			if (i + offsetY < 0 || i + offsetY >= height)
				continue;
			if (j + offsetX < 0 || j + offsetX >= width)
				continue;

			//赋值
			pBitmap[1][i*width + j] = pBitmap[0][(i + offsetY)*width + (j + offsetX)];
		}
	}
}