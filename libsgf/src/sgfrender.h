#pragma once

#include "sgfbitmappixels.h"
#include <Windows.h>


namespace SGF
{

//简单抽象
typedef void TextFormat;
typedef void Bitmap;


//字体样式
enum class FontStyle
{
	SFS_NORMAL,
	SFS_OBLIQUE,
	SFS_ITALIC
};


//字体粗细
enum class FontWeight
{
	SFW_THIN = 100,
	SFW_NORMAL = 400,
	SFW_MEDIUM = 500,
	SFW_BOLD = 700,
	SFW_HEAVY = 900
};


class Render
{
public:
	virtual ~Render() {};

	virtual int initiate(HWND hwnd) = 0;									//初始化
	virtual void uninitiate() = 0;											//反初始化

	virtual bool beginRender() = 0;
	virtual void renderFPS(float dt) = 0;									//开始渲染
	virtual void endRender() = 0;

	virtual void resize(unsigned int w, unsigned int h) = 0;				//调整大小
	virtual void focusChange(bool act) = 0;									//改变焦点

	virtual void getDesktopDpi(float* dpiX, float* dpiY) = 0;
	virtual void getSize(SIZE& size) = 0;


	//加载资源位图
	virtual int loadBitmapFromResource(
		HINSTANCE hinstance,
		PCWSTR resourceName,
		PCWSTR resourceType,
		UINT destinationWidth,
		UINT destinationHeight,
		Bitmap **ppBitmap) = 0;


	//加载文件位图
	virtual int loadBitmapFromFile(
		PCWSTR filename,
		UINT destinationWidth,
		UINT destinationHeight,
		Bitmap **ppBitmap) = 0;


	virtual int loadBitmapFromFile(
		PCWSTR filename,
		UINT destinationWidth,
		UINT destinationHeight,
		BitmapPixels* pPixels) = 0;


	//绘制图像
	virtual void drawBitmap(Bitmap* bitmap, int x, int y) = 0;

	//获取位图格式
	virtual BitmapProperties getBitmapProperties(Bitmap* bitmap) = 0;

	//创建图像
	virtual int createBitmap(unsigned int w, unsigned int h, void* data, BitmapProperties* props, Bitmap **ppBitmap) = 0;

	//Copy图像
	virtual int copyBmpToBmp(Bitmap* dst, unsigned int x, unsigned int y,
		Bitmap *src, unsigned int w, unsigned int h) = 0;

	virtual int copyMemToBmp(Bitmap* dst, RECT& rect, void* src, unsigned int pitch) = 0;


public:
	//未完善接口
	virtual TextFormat* createTextFormat(const TCHAR* fontfamily, int fontsize,
		FontStyle fontstyle, FontWeight fontweight) = 0;
	virtual void destoryTextFormat(TextFormat*) = 0;
	virtual void setTextColor(TextFormat* format, DWORD rgb) = 0;
	virtual void drawText(int x, int y, const TCHAR* text, const TextFormat* format) = 0;
};

}