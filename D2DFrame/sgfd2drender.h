#pragma once

#include <Windows.h>
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>
#include <memory>
#include "sgferr.h"
#include "sgfbitmappixels.h"

namespace SGF
{
	class GameApp;

	class D2DRender
	{
	public:
		D2DRender(const std::shared_ptr<GameApp>& app);
		virtual ~D2DRender();

		D2DRender(const D2DRender&) = delete;
		D2DRender& operator=(const D2DRender&) = delete;


	public:
		virtual ERR_TYPE initiate();
		virtual void uninitiate();

		virtual bool beginRender();
		virtual void renderFPS(float dt);
		virtual void endRender();

		virtual void resize(unsigned int w, unsigned int h);
		virtual void focusChange(bool act);
		virtual void getDesktopDpi(float* dpiX, float* dpiY);
		virtual void getSize(SIZE& size);

	public:
		//加载文件位图
		virtual int loadBitmapFromFile(
			PCWSTR filename,
			UINT destinationWidth,
			UINT destinationHeight,
			ID2D1Bitmap **ppBitmap);
		virtual int loadBitmapFromFile(
			PCWSTR filename,
			UINT destinationWidth,
			UINT destinationHeight,
			BitmapPixels* pPixels);
		virtual BitmapProperties getBitmapProperties(ID2D1Bitmap* bitmap);
		virtual int createBitmap(unsigned int w, unsigned int h, void* data, BitmapProperties* props, ID2D1Bitmap **ppBitmap);
		virtual int copyMemToBmp(ID2D1Bitmap* dst, const RECT& rect, void* src, unsigned int pitch);
		virtual void drawBitmap(float dstx, float dsty, ID2D1Bitmap* bitmap, float opacity = 1.0f);
		virtual void drawBitmap(float dstx, float dsty, ID2D1Bitmap* bitmap,
			float srcx, float srcy, float srcw, float srch, float opacity = 1.0f);
		virtual void drawBitmap(float dstx, float dsty, float dstw, float dsth,
			ID2D1Bitmap* bitmap, float srcx, float srcy, float srcw, float srch, float opacity = 1.0f);

	public:
		//返回原始指针
		ID2D1Factory * getFactory() const { return _direct2dFactory; }
		IDWriteFactory* getWriteFactory() const { return _dWriteFactory; }
		ID2D1HwndRenderTarget* getHwndRenderTarget() const { return _renderTarget;  }
		void showFPS(bool show = true) { _isFPS = show; }

	private:
		ERR_TYPE createDeviceResources();
		void discardDeviceResources();
		ERR_TYPE createDeviceIndependentResources();
		void discardDeviceIndependentResources();


	private:
		//设备独立资源
		ID2D1Factory * _direct2dFactory;
		IWICImagingFactory *_wicFactory;
		IDWriteFactory *_dWriteFactory;
		IDWriteTextFormat *_textFormat;

		//设备相关资源
		ID2D1HwndRenderTarget* _renderTarget;
		ID2D1SolidColorBrush* _whiteBrush;


	private:
		std::shared_ptr<GameApp> _baseApp;
		bool _isFPS;
		unsigned int _fps;
		unsigned int _fpsCount;
		float _totalTime;
	};


	template<class Interface>
	inline void D2DSafeRelease(
		Interface **ppInterfaceToRelease
	)
	{
		if (*ppInterfaceToRelease != NULL)
		{
			(*ppInterfaceToRelease)->Release();
			(*ppInterfaceToRelease) = NULL;
		}
	}
}