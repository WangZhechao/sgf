#pragma once

#include <memory>

namespace SGF
{

struct PixelFormat
{
	int dxgi_format;
	int alpha_mode;
};

struct BitmapProperties
{
	PixelFormat pixel_format;
	float dpiX;
	float dpiY;
	float width;
	float height;
};


struct BitmapPixels
{
	BitmapProperties props;

	unsigned char *data;
	unsigned int size;

public:
	BitmapPixels()
		:data(NULL), size(0)
	{
		memset(&props, 0, sizeof(BitmapProperties));
	}

	~BitmapPixels()
	{
		Release();
	}

public:
	void Release()
	{
		if (data) {
			delete[]data;
			data = NULL;
			size = 0;
			memset(&props, 0, sizeof(BitmapProperties));
		}
	}
};

}