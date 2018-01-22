#pragma once

#include "sgfbitmappixels.h"
#include <Windows.h>


namespace SGF
{

//�򵥳���
typedef void TextFormat;
typedef void Bitmap;


//������ʽ
enum class FontStyle
{
	SFS_NORMAL,
	SFS_OBLIQUE,
	SFS_ITALIC
};


//�����ϸ
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

	virtual int initiate(HWND hwnd) = 0;									//��ʼ��
	virtual void uninitiate() = 0;											//����ʼ��

	virtual bool beginRender() = 0;
	virtual void renderFPS(float dt) = 0;									//��ʼ��Ⱦ
	virtual void endRender() = 0;

	virtual void resize(unsigned int w, unsigned int h) = 0;				//������С
	virtual void focusChange(bool act) = 0;									//�ı佹��

	virtual void getDesktopDpi(float* dpiX, float* dpiY) = 0;
	virtual void getSize(SIZE& size) = 0;


	//������Դλͼ
	virtual int loadBitmapFromResource(
		HINSTANCE hinstance,
		PCWSTR resourceName,
		PCWSTR resourceType,
		UINT destinationWidth,
		UINT destinationHeight,
		Bitmap **ppBitmap) = 0;


	//�����ļ�λͼ
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


	//����ͼ��
	virtual void drawBitmap(int x, int y, Bitmap* bitmap) = 0;
	virtual void drawBitmap(int dx, int dy, Bitmap* bitmap,
		int sx, int sy, int sw, int sh) = 0;
	virtual void drawBitmap(int dx, int dy, int dw, int dh,
		Bitmap* bitmap, int sx, int sy, int sw, int sh) = 0;

	//��ȡλͼ��ʽ
	virtual BitmapProperties getBitmapProperties(Bitmap* bitmap) = 0;

	//����ͼ��
	virtual int createBitmap(unsigned int w, unsigned int h, void* data, BitmapProperties* props, Bitmap **ppBitmap) = 0;

	//Copyͼ��
	virtual int copyBmpToBmp(Bitmap* dst, unsigned int x, unsigned int y,
		Bitmap *src, unsigned int w, unsigned int h) = 0;

	virtual int copyMemToBmp(Bitmap* dst, const RECT& rect, void* src, unsigned int pitch) = 0;


public:
	//δ���ƽӿ�
	virtual TextFormat* createTextFormat(const TCHAR* fontfamily, int fontsize,
		FontStyle fontstyle, FontWeight fontweight) = 0;
	virtual void destoryTextFormat(TextFormat*) = 0;
	virtual void setTextColor(TextFormat* format, DWORD rgb) = 0;
	virtual void drawText(int x, int y, const TCHAR* text, const TextFormat* format) = 0;
};

}