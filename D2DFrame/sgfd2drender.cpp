#include "sgfd2drender.h"
#include "sgfbasewindow.h"
#include "sgfgameapp.h"
#include <tchar.h>
#include <assert.h>

#pragma comment(lib, "dwrite")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

namespace SGF
{
	const float DEFAULT_SYSTEM_DPI = 96.0f;
	const int DEFAULT_FPS = 30;

	D2DRender::D2DRender(const std::shared_ptr<GameApp>& app)
		: _direct2dFactory(NULL),
		_wicFactory(NULL),
		_dWriteFactory(NULL),
		_textFormat(NULL),
		_renderTarget(NULL),
		_whiteBrush(NULL),
		_baseApp(app), 
		_isFPS(true), 
		_fps(DEFAULT_FPS),
		_fpsCount(0), 
		_totalTime(0.0f)
	{

	}


	D2DRender::~D2DRender()
	{

	}


	void D2DRender::uninitiate()
	{
		discardDeviceResources();
		discardDeviceIndependentResources();
		CoUninitialize();
	}


	ERR_TYPE D2DRender::initiate()
	{
		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr)) {
			return ERR_COM_INIT_FAIL;
		}

		ERR_TYPE ret = createDeviceIndependentResources();
		if (ret != ERR_NONE) {
			uninitiate();
			return ret;
		}

		ret = createDeviceResources();
		if (ret != ERR_NONE) {
			uninitiate();
			return ret;
		}

		return ret;
	}


	ERR_TYPE D2DRender::createDeviceIndependentResources()
	{
		HRESULT hr = S_OK;

		//创建工厂
		if (!_direct2dFactory)
			hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_direct2dFactory);


		if (!_wicFactory)
		{
			//创建wic工厂
			if (SUCCEEDED(hr))
			{
				hr = CoCreateInstance(CLSID_WICImagingFactory, NULL,
					CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
					reinterpret_cast<void **>(&_wicFactory));
			}

			//WIN7 注册失败
			if (FAILED(hr))
			{
				hr = CoCreateInstance(CLSID_WICImagingFactory1, NULL,
					CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
					reinterpret_cast<void **>(&_wicFactory));
			}
		}

		static const WCHAR msc_fontName[] = L"Courier New";
		static const FLOAT msc_fontSize = 22;

		if (SUCCEEDED(hr) && (!_dWriteFactory))
		{
			// Create a DirectWrite factory.
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(_dWriteFactory),
				reinterpret_cast<IUnknown **>(&_dWriteFactory)
			);
		}

		if (SUCCEEDED(hr) && (!_textFormat))
		{
			// Create a DirectWrite text format object.
			hr = _dWriteFactory->CreateTextFormat(
				msc_fontName,
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				msc_fontSize,
				L"", //locale
				&_textFormat
			);


			if (SUCCEEDED(hr) && _textFormat)
			{
				// Center the text horizontally and vertically.
				_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			}
		}

		return (SUCCEEDED(hr) ? ERR_NONE : ERR_CREATE_D2D_INDEPENDENT_DEVICE_FAIL);
	}


	void D2DRender::discardDeviceIndependentResources()
	{
		D2DSafeRelease(&_textFormat);
		D2DSafeRelease(&_dWriteFactory);
		D2DSafeRelease(&_direct2dFactory);
		D2DSafeRelease(&_wicFactory);
	}


	ERR_TYPE D2DRender::createDeviceResources()
	{
		HRESULT hr = S_OK;

		if (!_renderTarget)
		{
			//依赖 Window
			assert(_baseApp && _baseApp->getWindow());

			HWND hwnd = _baseApp->getWindow()->getHWND();

			RECT rc;
			GetClientRect(hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
			);

			if (!_direct2dFactory)
				return ERR_CREATE_D2D_DEVICE_FAIL;

			hr = _direct2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
				&_renderTarget);

			if (SUCCEEDED(hr) && _renderTarget)
			{
				hr = _renderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::White),
					&_whiteBrush);
			}
		}

		return (SUCCEEDED(hr) ? ERR_NONE : ERR_CREATE_D2D_DEVICE_FAIL);
	}


	void D2DRender::discardDeviceResources()
	{
		D2DSafeRelease(&_whiteBrush);
		D2DSafeRelease(&_renderTarget);
	}


	bool D2DRender::beginRender()
	{
		//创建设备资源
		HRESULT hr = createDeviceResources();

		if (SUCCEEDED(hr) && _renderTarget)
		{
			_renderTarget->BeginDraw();

			//绘制
			_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));


			return true;
		}

		return false;
	}


	void D2DRender::endRender()
	{
		if (_renderTarget)
		{
			HRESULT hr = _renderTarget->EndDraw();

			/*
			* 如果设备丢失
			* 程序在全屏状态下失去键盘焦点（全屏时按下Alt+Tab或Win+D键或Win+L键）
			* 其他程序进入全屏状态
			* 电源管理事件，比如屏保等
			*/
			if (hr == D2DERR_RECREATE_TARGET)
			{
				discardDeviceResources();
			}
		}
	}

	void D2DRender::renderFPS(float dt)
	{
		if (_renderTarget)
		{
			//累计时间
			_totalTime += dt;

			//计算帧
			if (_totalTime >= 1000.0f)
			{
				_totalTime -= 1000.0f;
				_fps = _fpsCount;
				_fpsCount = 1;
			}
			else
			{
				++_fpsCount;
			}



			if (_isFPS && _textFormat && _renderTarget)
			{
				TCHAR tfps[32] = { 0 };
				_stprintf(tfps, TEXT("FPS: %d"), _fps);
				_renderTarget->DrawText(
					tfps,
					_tcsclen(tfps),
					_textFormat,
					D2D1::RectF(0, 0, 300, 50),
					_whiteBrush
				);
			}
		}
	}

	//改变大小
	void D2DRender::resize(unsigned int width, unsigned int height)
	{
		if (_renderTarget)
		{
			// Note: This method can fail, but it's okay to ignore the
			// error here, because the error will be returned again
			// the next time EndDraw is called.
			_renderTarget->Resize(D2D1::SizeU(width, height));
		}
	}


	//焦点改变
	void D2DRender::focusChange(bool act)
	{
	}


	//获取dpi
	void D2DRender::getDesktopDpi(float* dpiX, float* dpiY)
	{
		HRESULT hr = createDeviceIndependentResources();
		if (SUCCEEDED(hr) && _direct2dFactory)
		{
			_direct2dFactory->GetDesktopDpi(dpiX, dpiY);
		}
	}

	void D2DRender::getSize(SIZE& size)
	{
		if (_renderTarget)
		{
			D2D1_SIZE_F sizef = _renderTarget->GetSize();
			size = { (long)sizef.width, (long)sizef.height };
		}
		else
		{
			size = { 0, 0 };
		}
	}


	int D2DRender::loadBitmapFromFile(PCWSTR filename, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap ** ppBitmap)
	{
		HRESULT hr = S_OK;

		IWICBitmapDecoder *pDecoder = NULL;
		IWICBitmapFrameDecode *pSource = NULL;
		IWICStream *pStream = NULL;
		IWICFormatConverter *pConverter = NULL;
		IWICBitmapScaler *pScaler = NULL;

		if (!_wicFactory) {
			return -1;
		}


		hr = _wicFactory->CreateDecoderFromFilename(
			filename,
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);

		if (SUCCEEDED(hr))
		{
			// Create the initial frame.
			hr = pDecoder->GetFrame(0, &pSource);
		}

		if (SUCCEEDED(hr))
		{
			// Convert the image format to 32bppPBGRA
			// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
			hr = _wicFactory->CreateFormatConverter(&pConverter);
		}


		if (SUCCEEDED(hr))
		{
			// If a new width or height was specified, create an
			// IWICBitmapScaler and use it to resize the image.
			if (destinationWidth != 0 || destinationHeight != 0)
			{
				UINT originalWidth, originalHeight;
				hr = pSource->GetSize(&originalWidth, &originalHeight);
				if (SUCCEEDED(hr))
				{
					if (destinationWidth == 0)
					{
						FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
						destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
					}
					else if (destinationHeight == 0)
					{
						FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
						destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
					}

					hr = _wicFactory->CreateBitmapScaler(&pScaler);
					if (SUCCEEDED(hr))
					{
						hr = pScaler->Initialize(
							pSource,
							destinationWidth,
							destinationHeight,
							WICBitmapInterpolationModeCubic
						);
					}
					if (SUCCEEDED(hr))
					{
						hr = pConverter->Initialize(
							pScaler,
							GUID_WICPixelFormat32bppPBGRA,
							WICBitmapDitherTypeNone,
							NULL,
							0.f,
							WICBitmapPaletteTypeMedianCut
						);
					}
				}
			}
			else // Don't scale the image.
			{
				hr = pConverter->Initialize(
					pSource,
					GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone,
					NULL,
					0.f,
					WICBitmapPaletteTypeMedianCut
				);
			}
		}

		if (SUCCEEDED(hr))
		{
			//create a Direct2D bitmap from the WIC bitmap.
			hr = _renderTarget->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				(ID2D1Bitmap**)ppBitmap
			);
		}


		D2DSafeRelease(&pDecoder);
		D2DSafeRelease(&pSource);
		D2DSafeRelease(&pStream);
		D2DSafeRelease(&pConverter);
		D2DSafeRelease(&pScaler);


		if (hr == S_OK)
			return 0;
		else
			return -2;
	}



	int D2DRender::loadBitmapFromFile(
		PCWSTR filename,
		UINT destinationWidth,
		UINT destinationHeight,
		BitmapPixels* pPixels)
	{
		HRESULT hr = S_OK;

		IWICBitmapDecoder *pDecoder = NULL;
		IWICBitmapFrameDecode *pSource = NULL;
		IWICStream *pStream = NULL;
		IWICFormatConverter *pConverter = NULL;
		IWICBitmapScaler *pScaler = NULL;
		IWICBitmap *pBitmap = NULL;

		if (!_wicFactory) {
			return -1;
		}


		hr = _wicFactory->CreateDecoderFromFilename(
			filename,
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);

		if (SUCCEEDED(hr))
		{
			// Create the initial frame.
			hr = pDecoder->GetFrame(0, &pSource);
		}

		if (SUCCEEDED(hr))
		{
			// Convert the image format to 32bppPBGRA
			// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
			hr = _wicFactory->CreateFormatConverter(&pConverter);
		}


		if (SUCCEEDED(hr))
		{
			// If a new width or height was specified, create an
			// IWICBitmapScaler and use it to resize the image.
			if (destinationWidth != 0 || destinationHeight != 0)
			{
				UINT originalWidth, originalHeight;
				hr = pSource->GetSize(&originalWidth, &originalHeight);
				if (SUCCEEDED(hr))
				{
					if (destinationWidth == 0)
					{
						FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
						destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
					}
					else if (destinationHeight == 0)
					{
						FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
						destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
					}

					hr = _wicFactory->CreateBitmapScaler(&pScaler);
					if (SUCCEEDED(hr))
					{
						hr = pScaler->Initialize(
							pSource,
							destinationWidth,
							destinationHeight,
							WICBitmapInterpolationModeCubic
						);
					}
					if (SUCCEEDED(hr))
					{
						hr = pConverter->Initialize(
							pScaler,
							GUID_WICPixelFormat32bppPBGRA,
							WICBitmapDitherTypeNone,
							NULL,
							0.f,
							WICBitmapPaletteTypeMedianCut
						);
					}
				}
			}
			else // Don't scale the image.
			{
				hr = pConverter->Initialize(
					pSource,
					GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone,
					NULL,
					0.f,
					WICBitmapPaletteTypeMedianCut
				);
			}
		}


		UINT width = 0, height = 0;

		if (SUCCEEDED(hr))
		{
			hr = pConverter->GetSize(&width, &height);
		}

		if (SUCCEEDED(hr))
		{
			hr = _wicFactory->CreateBitmapFromSourceRect(
				pConverter, 0, 0, width, height, &pBitmap);
		}

		IWICBitmapLock *pILock = NULL;
		WICRect rcLock = { 0, 0, width, height };
		if (SUCCEEDED(hr))
		{
			hr = pBitmap->Lock(&rcLock, WICBitmapLockRead, &pILock);
		}


		if (SUCCEEDED(hr))
		{
			BYTE *pv = NULL;
			UINT size = 0;

			hr = pILock->GetDataPointer(&size, (WICInProcPointer*)&pv);
			if (SUCCEEDED(hr))
			{
				if (pPixels)
				{
					pPixels->data = new unsigned char[size];
					memcpy(pPixels->data, pv, size);

					pPixels->size = size;
				}

				pILock->Release();
			}
		}


		if (SUCCEEDED(hr))
		{
			ID2D1Bitmap *pBmp = NULL;
			//create a Direct2D bitmap from the WIC bitmap.
			hr = _renderTarget->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				&pBmp
			);

			pPixels->props = getBitmapProperties(pBmp);
			D2DSafeRelease(&pBmp);
		}


		D2DSafeRelease(&pDecoder);
		D2DSafeRelease(&pSource);
		D2DSafeRelease(&pStream);
		D2DSafeRelease(&pConverter);
		D2DSafeRelease(&pScaler);
		D2DSafeRelease(&pBitmap);


		if (hr == S_OK)
			return 0;
		else
			return -2;
	}


	BitmapProperties D2DRender::getBitmapProperties(ID2D1Bitmap* pBitmap)
	{
		BitmapProperties props = { 0 };

		memcpy(&props.pixel_format, &pBitmap->GetPixelFormat(), sizeof(D2D1_PIXEL_FORMAT));

		D2D1_SIZE_F sizef = pBitmap->GetSize();
		props.width = sizef.width;
		props.height = sizef.height;

		pBitmap->GetDpi(&props.dpiX, &props.dpiY);

		return props;
	}


	int D2DRender::createBitmap(unsigned int w, unsigned int h, void* data, BitmapProperties* props, ID2D1Bitmap **ppBitmap)
	{
		if (_renderTarget == NULL) {
			(*ppBitmap) = NULL;
			return -1;
		}

		if (w == 0)
			w = props->width;

		if (h == 0)
			h = props->height;


		D2D1_SIZE_U size = { w, h };
		UINT32 pitch = size.width * 4;
		D2D1_BITMAP_PROPERTIES bitmapFormat;
		memcpy(&bitmapFormat, props, sizeof(D2D1_BITMAP_PROPERTIES));


		HRESULT hr = _renderTarget->CreateBitmap(size, data, pitch, bitmapFormat, ppBitmap);
		if (hr == S_OK) {
			return 0;
		}

		return -2;
	}


	int D2DRender::copyMemToBmp(ID2D1Bitmap* dst, const RECT& rt, void* src, unsigned int pitch)
	{
		ID2D1Bitmap *dstBmp = (ID2D1Bitmap *)dst;
		if (dstBmp == NULL || src == NULL) {
			return -1;
		}

		D2D1_RECT_U rect = { 0 };
		memcpy(&rect, &rt, sizeof(rect));

		HRESULT hr = dstBmp->CopyFromMemory(&rect, src, pitch);
		if (hr == S_OK) {
			return 0;
		}

		return -2;
	}


	void D2DRender::drawBitmap(float dstx, float dsty, ID2D1Bitmap* bitmap, float opacity)
	{
		if (_renderTarget == NULL || bitmap == NULL)
			return;

		ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

		D2D1_SIZE_F size = pBitmap->GetSize();

		_renderTarget->DrawBitmap(pBitmap, D2D1::RectF(dstx, dsty, dstx + size.width, dsty + size.height), opacity);
	}


	void D2DRender::drawBitmap(float dstx, float dsty, ID2D1Bitmap* bitmap,
		float srcx, float srcy, float srcw, float srch, float opacity)
	{
		if (_renderTarget == NULL || bitmap == NULL)
			return;

		ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

		D2D1_SIZE_F size = pBitmap->GetSize();
		if (srcw <= 0)
			srcw = size.width;
		if (srch <= 0)
			srch = size.height;

		_renderTarget->DrawBitmap(pBitmap, D2D1::RectF(dstx, dsty, dstx + srcw, dsty + srch),
			opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(srcx, srcy, srcx + srcw, srcy + srch));

	}


	void D2DRender::drawBitmap(float dstx, float dsty, float dstw, float dsth,
		ID2D1Bitmap* bitmap, float srcx, float srcy, float srcw, float srch, float opacity)
	{
		if (_renderTarget == NULL || bitmap == NULL)
			return;

		ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

		D2D1_SIZE_F size = pBitmap->GetSize();
		if (srcw <= 0) srcw = size.width;
		if (srch <= 0) srch = size.height;

		if (dstw <= 0) dstw = srcw;
		if (dsth <= 0) dsth = srch;

		_renderTarget->DrawBitmap(pBitmap, D2D1::RectF(dstx, dsty, dstx + dstw, dsty + dsth),
			opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(srcx, srcy, srcx + srcw, srcy + srch));

	}
}