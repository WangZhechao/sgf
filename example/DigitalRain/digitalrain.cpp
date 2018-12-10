#include "digitalrain.h"
#include "sgfbasewindow.h"
#include "sgfd2drender.h"
#include "sgfeventdispatcher.h"
#include "strline.h"
#include <tchar.h>

#pragma comment(lib, "dwrite")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

const int DEFAULT_FONT_HEIGHT = 12;
const int DEFAULT_FONT_WIDTH = 15;


DigitalRain::DigitalRain(HINSTANCE h)
	: SGF::GameApp(h), _writeBrush(NULL), _writeformat(NULL)
{
	_win = { 800, 600 };
	_font = { DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT };

	srand((int)GetTickCount()); //初始化随机数发生器
}


DigitalRain::~DigitalRain()
{

}


int DigitalRain::gameInitiate()
{
	_d2dRender->getSize(_win);
	_baseWindow->setGameTitle(TEXT("数字雨"));

	int ret = createFont();
	if (ret < 0) {
		return ret;
	}

	genCharLines();

	return 0;
}


void DigitalRain::gameUpdate(float dt)
{
	std::vector<StrLine>::iterator itr;

	for (itr = _numLines.begin(); itr != _numLines.end(); ++itr)
	{
		//降低等待时间
		if (itr->_waiTime > 0)
		{
			itr->_waiTime--;
		}
		//删除旧的添加新的
		else
		{
			itr->_charList.push_front(itr->genChar());
			itr->_charList.pop_back();

			itr->_beginPos.y += itr->_vy;
		}


		//越界
		if (itr->_beginPos.y - (itr->_length * _font.cy) > _win.cy)
		{
			itr->genLine();
		}
	}
}


void DigitalRain::gameRender(float lag)
{
	TCHAR ctmp[2] = { 0 };

	std::vector<StrLine>::iterator vitr;
	for (vitr = _numLines.begin(); vitr != _numLines.end(); ++vitr)
	{
		if (vitr->_waiTime > 0)
			continue;


		std::list<char>::iterator litr;
		int count = 1;
		for (litr = vitr->_charList.begin(); litr != vitr->_charList.end(); ++litr)
		{
			POINT point = { vitr->_beginPos.x, vitr->_beginPos.y - _font.cy * count };


			if (count == 1)
				_writeBrush->SetColor(D2D1::ColorF(RGB(255, 255, 255)));
			else
				_writeBrush->SetColor(D2D1::ColorF(RGB(0, 255 - 255 / (vitr->_length)*count, 0)));


			if ((point.x >= 0 && point.x <= _win.cx - _font.cx)
				&& (point.y >= 0 && point.y <= _win.cy - _font.cy))
			{
				ctmp[0] = *litr;

				ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
				const D2D1_SIZE_F &size = renderTarget->GetSize();
				renderTarget->DrawText(ctmp, _tcsclen(ctmp), _writeformat,
					D2D1::RectF(point.x, point.y, size.width, size.height), _writeBrush);
			}

			count++;
		}
	}
}


void DigitalRain::gameUninitiate()
{
	destoryFont();
}


int DigitalRain::createFont()
{
	IDWriteFactory* dWriteFactory = _d2dRender->getWriteFactory();
	if (!dWriteFactory) {
		return -1;
	}


	ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
	if (!renderTarget) {
		return -2;
	}


	HRESULT hr = dWriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_HEAVY,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_FONT_WIDTH, L"", &_writeformat);
	if (FAILED(hr)) {
		return -3;
	}


	hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_writeBrush);
	if (FAILED(hr)) {
		SGF::D2DSafeRelease(&_writeformat);
		return -4;
	}

	return 0;
}


void DigitalRain::destoryFont()
{
	SGF::D2DSafeRelease(&_writeBrush);
	SGF::D2DSafeRelease(&_writeformat);
}


void DigitalRain::genCharLines()
{
	int count = _win.cx / _font.cx;

	_numLines.clear();
	for (int i = 0; i < count; i++)
	{
		_numLines.push_back(StrLine(i * _font.cx, rand()%_win.cy));
	}
}