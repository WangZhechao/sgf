#include <tchar.h>
#include "sgfd2drender.h"
#include "sgferr.h"

#pragma comment(lib, "dwrite")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")

namespace SGF
{

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


struct D2DTextFormat
{
	ID2D1SolidColorBrush* textBrush;
	IDWriteTextFormat *textFormat;
};


D2DRender::D2DRender() :
	_isFPS(true),
	_fps(0),
	_fpsCount(0),
	_totalTime(0.0f),
	_blackBrush(NULL),
	_dWriteFactory(NULL),
	_textFormat(NULL),
	_wicFactory(NULL),
	_wnd(NULL),
	_direct2dFactory(NULL),
	_renderTarget(NULL)
{
	create();
}

D2DRender::~D2DRender()
{
	destory();
}

//创建
int D2DRender::create()
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return ERR_COM_INIT_FAIL;
	}

	hr = createDeviceIndependentResources();
	if (FAILED(hr))
	{
		CoUninitialize();
		return ERR_CREATE_D2D_DEVICE_FAIL;
	}

	return hr;
}

//销毁
void D2DRender::destory()
{
	discardDeviceIndependentResources();
	CoUninitialize();
}


//初始化
int D2DRender::initiate(HWND hwnd)
{
	_wnd = hwnd;

	HRESULT hr = createDeviceResources();
	if (FAILED(hr))
	{
		return ERR_INITIATE_D2D_DEVICE_FAIL;
	}

	return ERR_NONE;
}


//反初始化
void D2DRender::uninitiate()
{
	discardDeviceResources();
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
				_blackBrush
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



int D2DRender::loadBitmapFromResource(HINSTANCE hinstance, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, Bitmap ** ppBitmap)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;


	if (!_wicFactory) {
		return -1;
	}

	// Locate the resource.
	imageResHandle = FindResourceW(hinstance, resourceName, resourceType);

	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(hinstance, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}


	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}


	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(hinstance, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}


	if (SUCCEEDED(hr) && _wicFactory)
	{
		// Create a WIC stream to map onto the memory.
		hr = _wicFactory->CreateStream(&pStream);
	}


	if (SUCCEEDED(hr) && pStream)
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
		);
	}


	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = _wicFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}


	if (SUCCEEDED(hr) && pDecoder)
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
		}
		else
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



int D2DRender::loadBitmapFromFile(PCWSTR filename, UINT destinationWidth, UINT destinationHeight, Bitmap ** ppBitmap)
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


void D2DRender::drawBitmap(int x, int y, Bitmap* bitmap)
{
	if (_renderTarget == NULL || bitmap == NULL)
		return;

	ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

	D2D1_SIZE_F size = pBitmap->GetSize();

	_renderTarget->DrawBitmap(pBitmap, D2D1::RectF(x, y, x+size.width, y+size.height));
}


void D2DRender::drawBitmap(int dx, int dy, Bitmap* bitmap,
	int sx, int sy, int sw, int sh)
{
	if (_renderTarget == NULL || bitmap == NULL)
		return;

	ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

	D2D1_SIZE_F size = pBitmap->GetSize();
	if (sw <= 0)
		sw = size.width;
	if (sh <= 0)
		sh = size.height;

	_renderTarget->DrawBitmap(pBitmap, D2D1::RectF(dx, dy, dx+sw, dy+sh),
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(sx, sy, sx+sw, sy+sh));
}

void D2DRender::drawBitmap(int dx, int dy, int dw, int dh, Bitmap * bitmap, int sx, int sy, int sw, int sh)
{
	if (_renderTarget == NULL || bitmap == NULL)
		return;

	ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

	D2D1_SIZE_F size = pBitmap->GetSize();
	if (sw <= 0) sw = size.width;
	if (sh <= 0) sh = size.height;

	if (dw <= 0) dw = sw;
	if (dh <= 0) dh = sh;

	_renderTarget->DrawBitmap(pBitmap, D2D1::RectF(dx, dy, dx+dw, dy+dh),
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(sx, sy, sx+sw, sy+sh));
}


BitmapProperties D2DRender::getBitmapProperties(Bitmap * bitmap)
{
	BitmapProperties props = { 0 };
	ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

	if (bitmap == NULL)
		return props;

	memcpy(&props.pixel_format, &pBitmap->GetPixelFormat(), sizeof(D2D1_PIXEL_FORMAT));

	D2D1_SIZE_F sizef = pBitmap->GetSize();
	props.width = sizef.width;
	props.height = sizef.height;

	pBitmap->GetDpi(&props.dpiX, &props.dpiY);

	return props;
}

int D2DRender::createBitmap(unsigned int w, unsigned int h, void *data, BitmapProperties * props, Bitmap ** ppBitmap)
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

	
	HRESULT hr = _renderTarget->CreateBitmap(size, data, pitch, bitmapFormat, (ID2D1Bitmap**)ppBitmap);
	if (hr == S_OK) {
		return 0;
	}

	return -2;
}

int D2DRender::copyBmpToBmp(Bitmap * desc, unsigned int x, unsigned int y, Bitmap * src, unsigned int w, unsigned int h)
{
	ID2D1Bitmap *descBmp = (ID2D1Bitmap *)desc;
	ID2D1Bitmap *srcBmp = (ID2D1Bitmap *)src;

	if (descBmp == NULL || srcBmp == NULL) {
		return -1;
	}


	D2D1_POINT_2U point = { x, y };
	D2D1_RECT_U rect = { 0, 0, w, h };

	HRESULT hr = descBmp->CopyFromBitmap(&point, srcBmp, &rect);
	if (hr == S_OK) {
		return 0;
	}

	return -2;
}


int D2DRender::copyMemToBmp(Bitmap* dst, const RECT& rt, void* src, unsigned int pitch)
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



//可重入
HRESULT D2DRender::createDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	if (!_direct2dFactory)
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_direct2dFactory);


	//这里很奇怪
	if (SUCCEEDED(hr) && (!_wicFactory))
	{
		// Create WIC factory.
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL,
			CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
			reinterpret_cast<void **>(&_wicFactory));
	}


	//WIN7 注册失败，再注册
	if (FAILED(hr) && (!_wicFactory))
	{
		// Create WIC factory.
		hr = CoCreateInstance(CLSID_WICImagingFactory1, NULL,
			CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
			reinterpret_cast<void **>(&_wicFactory));
	}


	if (_isFPS)
	{
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
	}

	return hr;
}


void D2DRender::discardDeviceIndependentResources()
{
	D2DSafeRelease(&_textFormat);
	D2DSafeRelease(&_dWriteFactory);
	D2DSafeRelease(&_direct2dFactory);
	D2DSafeRelease(&_wicFactory);
}



HRESULT D2DRender::createDeviceResources()
{
	HRESULT hr = S_OK;

	//如果没有创建设备资源
	if (!_renderTarget)
	{
		RECT rc;
		GetClientRect(_wnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);


		if (_direct2dFactory)
		{
			hr = _direct2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(_wnd, size),
				&_renderTarget);
		}


		if (_isFPS)
		{
			if (SUCCEEDED(hr) && _renderTarget)
			{
				hr = _renderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::White),
					&_blackBrush);
			}
		}
	}

	return hr;
}


void D2DRender::discardDeviceResources()
{
	D2DSafeRelease(&_blackBrush);
	D2DSafeRelease(&_renderTarget);
}



//未完善接口
TextFormat* D2DRender::createTextFormat(const TCHAR* fontfamily, int fontsize,
	FontStyle fontstyle, FontWeight fontweight)
{
	HRESULT hr = S_OK;

	/* 创建工厂 */
	if (!_dWriteFactory)
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(_dWriteFactory),
			reinterpret_cast<IUnknown **>(&_dWriteFactory)
		);
	}

	if (FAILED(hr))
		return NULL;

	IDWriteTextFormat* writeformat = NULL;
	hr = _dWriteFactory->CreateTextFormat(
		fontfamily,
		NULL,
		(DWRITE_FONT_WEIGHT)fontweight,
		(DWRITE_FONT_STYLE)fontstyle,
		DWRITE_FONT_STRETCH_NORMAL,
		fontsize,
		L"", //locale
		&writeformat
	);


	if (FAILED(hr))
	{
		return NULL;
	}


	//创建默认画刷
	ID2D1SolidColorBrush* writeBrush = NULL;
	hr = _renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&writeBrush);
	if (FAILED(hr))
	{
		D2DSafeRelease(&writeformat);
		return NULL;
	}


	/* 返回成功 */
	D2DTextFormat *format = new D2DTextFormat();
	format->textFormat = writeformat;
	format->textBrush = writeBrush;

	return (TextFormat*)format;
}


void D2DRender::destoryTextFormat(TextFormat* format)
{
	if (format == NULL)
		return;

	D2DTextFormat *d2dformat = (D2DTextFormat *)format;
	if (d2dformat)
	{
		D2DSafeRelease(&(d2dformat->textBrush));
		D2DSafeRelease(&(d2dformat->textFormat));

		delete d2dformat;
		d2dformat = NULL;
	}
}


void D2DRender::setTextColor(TextFormat* format, DWORD rgb)
{
	if (!format)
		return;

	D2DTextFormat *d2dformat = (D2DTextFormat *)format;
	if (d2dformat->textBrush)
		d2dformat->textBrush->SetColor(D2D1::ColorF(rgb));
}


void D2DRender::drawText(int x, int y, const TCHAR* text, const TextFormat* format)
{
	if (format == NULL || text == NULL || _tcsclen(text) <= 0)
		return;

	D2DTextFormat *d2dformat = (D2DTextFormat *)format;
	const D2D1_SIZE_F &size = _renderTarget->GetSize();

	_renderTarget->DrawText(
		text,
		_tcsclen(text),
		d2dformat->textFormat,
		D2D1::RectF(x, y, size.width, size.height),
		d2dformat->textBrush
	);
}



}