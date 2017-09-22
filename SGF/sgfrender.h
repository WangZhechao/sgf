#pragma once

typedef void TextFormat;
typedef void Bitmap;


enum SGF_FONT_STYLE
{
	SGF_FS_NORMAL,
	SGF_FS_OBLIQUE,
	SGF_FS_ITALIC
};

enum SGF_FONT_WEIGHT
{
	SGF_FW_THIN = 100,
	SGF_FW_NORMAL = 400,
	SGF_FW_MEDIUM = 500,
	SGF_FW_BOLD = 700,
	SGF_FW_HEAVY = 900
};

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
			:data(NULL),size(0)
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


	class Render
	{
	public:
		virtual ~Render() {};

		virtual int Initiate(HWND hwnd) = 0;									//初始化
		virtual void Uninitiate() = 0;											//反初始化

		virtual bool BeginRender() = 0;
		virtual void RenderFPS(float dt) = 0;									//开始渲染
		virtual void EndRender() = 0;

		virtual void Resize(unsigned int w, unsigned int h) = 0;				//调整大小
		virtual void FocusChange(bool act) = 0;									//改变焦点

		virtual void GetDesktopDpi(float* dpiX, float* dpiY) = 0;
		virtual void GetSize(SIZE& size) = 0;


		//加载资源位图
		virtual int LoadBitmapFromResource(
			HINSTANCE hinstance,
			PCWSTR resourceName,
			PCWSTR resourceType,
			UINT destinationWidth,
			UINT destinationHeight,
			Bitmap **ppBitmap) = 0;


		//加载文件位图
		virtual int LoadBitmapFromFile(
			PCWSTR filename,
			UINT destinationWidth,
			UINT destinationHeight,
			Bitmap **ppBitmap) = 0;


		virtual int LoadBitmapFromFile(
			PCWSTR filename,
			UINT destinationWidth,
			UINT destinationHeight,
			BitmapPixels* pPixels) = 0;


		//绘制图像
		virtual void DrawBitmap(Bitmap* bitmap, int x, int y) = 0;

		//获取位图格式
		virtual BitmapProperties GetBitmapProperties(Bitmap* bitmap) = 0;

		//创建图像
		virtual int CreateBitmap(unsigned int w, unsigned int h, void* data, BitmapProperties* props, Bitmap **ppBitmap) = 0;

		//Copy图像
		virtual int CopyBmpToBmp(Bitmap* dst, unsigned int x, unsigned int y,
			Bitmap *src, unsigned int w, unsigned int h) = 0;

		virtual int CopyMemToBmp(Bitmap* dst, RECT& rect, void* src, unsigned int pitch) = 0;


	public:
		//未完善接口
		virtual TextFormat* CreateTextFormat(const TCHAR* fontfamily, int fontsize, 
			SGF_FONT_STYLE fontstyle, SGF_FONT_WEIGHT fontweight) = 0;
		virtual void DestoryTextFormat(TextFormat*) = 0;
		virtual void SetTextColor(TextFormat* format, DWORD rgb) = 0;
		virtual void DrawText(int x, int y, const TCHAR* text, const TextFormat* format) = 0;
	};
}
