#pragma once

#include <Windows.h>
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>

#include "sgfrender.h"

namespace SGF
{

class D2DRender : public Render
{
public:
	D2DRender();
	virtual ~D2DRender();

public:
	virtual int initiate(HWND hwnd);
	virtual void uninitiate();

	virtual bool beginRender();
	virtual void renderFPS(float dt);
	virtual void endRender();

	virtual void resize(unsigned int w, unsigned int h);
	virtual void focusChange(bool act);
	virtual void getDesktopDpi(float* dpiX, float* dpiY);
	virtual void getSize(SIZE& size);

	virtual int loadBitmapFromResource(
		HINSTANCE hinstance,
		PCWSTR resourceName,
		PCWSTR resourceType,
		UINT destinationWidth,
		UINT destinationHeight,
		Bitmap **ppBitmap);


	//加载文件位图
	virtual int loadBitmapFromFile(
		PCWSTR filename,
		UINT destinationWidth,
		UINT destinationHeight,
		Bitmap **ppBitmap);


	virtual int loadBitmapFromFile(
		PCWSTR filename,
		UINT destinationWidth,
		UINT destinationHeight,
		BitmapPixels* pPixels);


	//绘制图像
	virtual void drawBitmap(Bitmap* bitmap, int x, int y);

	//获取位图格式
	virtual BitmapProperties getBitmapProperties(Bitmap* bitmap);

	//创建图像
	virtual int createBitmap(unsigned int w, unsigned int h, void* data, BitmapProperties* props, Bitmap **ppBitmap);


	//Copy图像
	virtual int copyBmpToBmp(Bitmap* desc, unsigned int x, unsigned int y,
		Bitmap *src, unsigned int w, unsigned int h);

	virtual int copyMemToBmp(Bitmap* dst, const RECT& rect, void* src, unsigned int pitch);


public:
	virtual TextFormat* createTextFormat(const TCHAR* fontfamily, int fontsize,
		FontStyle fontstyle, FontWeight fontweight);
	virtual void destoryTextFormat(TextFormat* format);

	virtual void setTextColor(TextFormat* format, DWORD rgb);
	virtual void drawText(int x, int y, const TCHAR* text, const TextFormat* format);


private:
	int create();
	void destory();

	HRESULT createDeviceIndependentResources();
	void discardDeviceIndependentResources();

	HRESULT createDeviceResources();
	void discardDeviceResources();


private:
	bool _isFPS;
	unsigned int _fps;
	unsigned int _fpsCount;
	float _totalTime;

	ID2D1SolidColorBrush* _blackBrush;
	IDWriteFactory *_dWriteFactory;
	IDWriteTextFormat *_textFormat;
	IWICImagingFactory *_wicFactory;

	HWND _wnd;
	ID2D1Factory* _direct2dFactory;
	ID2D1HwndRenderTarget* _renderTarget;
};


}