#include <tchar.h>
#include "sgfd2drender.h"
#include "sgferr.h"

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


	D2DRender::D2DRender():
		m_bFPS(true),
		m_nFPS(0),
		m_nFPSCount(0),
		m_lfTotalTime(0.0f),
		m_hWnd(NULL),
		m_pDirect2dFactory(NULL),
		m_pRenderTarget(NULL),
		m_pBlackBrush(NULL),
		m_pDWriteFactory(NULL),
		m_pTextFormat(NULL),
		m_pWICFactory(NULL)

	{
		Create();
	}

	D2DRender::~D2DRender()
	{
		Destory();
	}

	//创建
	int D2DRender::Create()
	{
		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr))
		{
			return ERR_COM_INIT_FAIL;
		}

		hr = CreateDeviceIndependentResources();
		if (FAILED(hr))
		{
			return ERR_CREATE_D2D_DEVICE_FAIL;
		}

		return hr;
	}

	void D2DRender::Destory()
	{
		CoUninitialize();
	}


	//初始化
	int D2DRender::Initiate(HWND hwnd)
	{
		m_hWnd = hwnd;

		HRESULT hr = CreateDeviceResources();
		if (FAILED(hr))
		{
			return ERR_INITIATE_D2D_DEVICE_FAIL;
		}

		return ERR_NONE;
	}


	//反初始化
	void D2DRender::Uninitiate()
	{
		DiscardDeviceResources();
		DiscardDeviceIndependentResources();
	}


	bool D2DRender::BeginRender()
	{
		//创建设备资源
		HRESULT hr = CreateDeviceResources();

		if (SUCCEEDED(hr) && m_pRenderTarget)
		{
			m_pRenderTarget->BeginDraw();

			//绘制
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));


			return true;
		}

		return false;
	}


	void D2DRender::EndRender()
	{
		if (m_pRenderTarget)
		{
			HRESULT hr = m_pRenderTarget->EndDraw();

			/*
			* 如果设备丢失
			* 程序在全屏状态下失去键盘焦点（全屏时按下Alt+Tab或Win+D键或Win+L键）
			* 其他程序进入全屏状态
			* 电源管理事件，比如屏保等
			*/
			if (hr == D2DERR_RECREATE_TARGET)
			{
				DiscardDeviceResources();
			}
		}
	}


	void D2DRender::RenderFPS(float dt)
	{
		if (m_pRenderTarget)
		{
			//累计时间
			m_lfTotalTime += dt;

			//计算帧
			if (m_lfTotalTime >= 1000.0f)
			{
				m_lfTotalTime -= 1000.0f;
				m_nFPS = m_nFPSCount;
				m_nFPSCount = 1;
			}
			else
			{
				++m_nFPSCount;
			}



			if (m_bFPS && m_pTextFormat)
			{
				TCHAR tfps[32] = { 0 };
				_stprintf(tfps, TEXT("FPS: %d"), m_nFPS);
				m_pRenderTarget->DrawText(
					tfps,
					_tcsclen(tfps),
					m_pTextFormat,
					D2D1::RectF(0, 0, 300, 50),
					m_pBlackBrush
				);
			}
		}
	}


	//改变大小
	void D2DRender::Resize(unsigned int width, unsigned int height)
	{
		if (m_pRenderTarget)
		{
			// Note: This method can fail, but it's okay to ignore the
			// error here, because the error will be returned again
			// the next time EndDraw is called.
			m_pRenderTarget->Resize(D2D1::SizeU(width, height));
		}
	}


	//焦点改变
	void D2DRender::FocusChange(bool act)
	{
	}


	//获取dpi
	void D2DRender::GetDesktopDpi(float* dpiX, float* dpiY)
	{
		HRESULT hr = CreateDeviceIndependentResources();
		if (SUCCEEDED(hr) && m_pDirect2dFactory)
		{
			m_pDirect2dFactory->GetDesktopDpi(dpiX, dpiY);
		}
	}

	void D2DRender::GetSize(SIZE& size)
	{
		if (m_pRenderTarget)
		{
			D2D1_SIZE_F sizef = m_pRenderTarget->GetSize();
			size = {(long)sizef.width, (long)sizef.height};
		}
		else
		{
			size = { 0, 0 };
		}
	}

	int D2DRender::LoadBitmapFromResource(HINSTANCE hinstance, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, Bitmap ** ppBitmap)
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


		if (!m_pWICFactory) {
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


		if (SUCCEEDED(hr) && m_pWICFactory)
		{
			// Create a WIC stream to map onto the memory.
			hr = m_pWICFactory->CreateStream(&pStream);
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
			hr = m_pWICFactory->CreateDecoderFromStream(
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
			hr = m_pWICFactory->CreateFormatConverter(&pConverter);
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

					hr = m_pWICFactory->CreateBitmapScaler(&pScaler);
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
			hr = m_pRenderTarget->CreateBitmapFromWicBitmap(
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

	int D2DRender::LoadBitmapFromFile(PCWSTR filename, UINT destinationWidth, UINT destinationHeight, Bitmap ** ppBitmap)
	{
		HRESULT hr = S_OK;

		IWICBitmapDecoder *pDecoder = NULL;
		IWICBitmapFrameDecode *pSource = NULL;
		IWICStream *pStream = NULL;
		IWICFormatConverter *pConverter = NULL;
		IWICBitmapScaler *pScaler = NULL;

		if (!m_pWICFactory) {
			return -1;
		}


		hr = m_pWICFactory->CreateDecoderFromFilename(
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
			hr = m_pWICFactory->CreateFormatConverter(&pConverter);
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

					hr = m_pWICFactory->CreateBitmapScaler(&pScaler);
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
			hr = m_pRenderTarget->CreateBitmapFromWicBitmap(
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

	void D2DRender::DrawBitmap(Bitmap * bitmap, int x, int y)
	{
		if (m_pRenderTarget == NULL || bitmap == NULL)
			return;

		ID2D1Bitmap *pBitmap = (ID2D1Bitmap *)bitmap;

		D2D1_SIZE_F size = pBitmap->GetSize();

		m_pRenderTarget->DrawBitmap(pBitmap, D2D1::RectF(x, y, size.width, size.height));
	}

	//可重入
	HRESULT D2DRender::CreateDeviceIndependentResources()
	{
		HRESULT hr = S_OK;

		if(!m_pDirect2dFactory)
			hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);


		if (SUCCEEDED(hr) && (!m_pWICFactory))
		{
			// Create WIC factory.
			hr = CoCreateInstance(CLSID_WICImagingFactory, NULL,
					CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
				reinterpret_cast<void **>(&m_pWICFactory));
		}


		if (m_bFPS)
		{
			static const WCHAR msc_fontName[] = L"Courier New";
			static const FLOAT msc_fontSize = 22;

			if (SUCCEEDED(hr) && (!m_pDWriteFactory))
			{
				// Create a DirectWrite factory.
				hr = DWriteCreateFactory(
					DWRITE_FACTORY_TYPE_SHARED,
					__uuidof(m_pDWriteFactory),
					reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
				);
			}


			if (SUCCEEDED(hr) && (!m_pTextFormat))
			{
				// Create a DirectWrite text format object.
				hr = m_pDWriteFactory->CreateTextFormat(
					msc_fontName,
					NULL,
					DWRITE_FONT_WEIGHT_NORMAL,
					DWRITE_FONT_STYLE_NORMAL,
					DWRITE_FONT_STRETCH_NORMAL,
					msc_fontSize,
					L"", //locale
					&m_pTextFormat
				);


				if (SUCCEEDED(hr) && m_pTextFormat)
				{
					// Center the text horizontally and vertically.
					m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
					m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
				}
			}
		}

		return hr;
	}


	void D2DRender::DiscardDeviceIndependentResources()
	{
		D2DSafeRelease(&m_pTextFormat);
		D2DSafeRelease(&m_pDWriteFactory);
		D2DSafeRelease(&m_pDirect2dFactory);
		D2DSafeRelease(&m_pWICFactory);
	}

	
	HRESULT D2DRender::CreateDeviceResources()
	{
		HRESULT hr = S_OK;

		//如果没有创建设备资源
		if (!m_pRenderTarget)
		{
			RECT rc;
			GetClientRect(m_hWnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
			);


			if (m_pDirect2dFactory)
			{
				hr = m_pDirect2dFactory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(m_hWnd, size),
					&m_pRenderTarget);
			}


			if (m_bFPS)
			{
				if (SUCCEEDED(hr) && m_pRenderTarget)
				{
					hr = m_pRenderTarget->CreateSolidColorBrush(
						D2D1::ColorF(D2D1::ColorF::White),
						&m_pBlackBrush);
				}
			}
		}

		return hr;
	}


	void D2DRender::DiscardDeviceResources()
	{
		D2DSafeRelease(&m_pBlackBrush);
		D2DSafeRelease(&m_pRenderTarget);
	}









	//未完善接口
	TextFormat* D2DRender::CreateTextFormat(const TCHAR* fontfamily, int fontsize,
		SGF_FONT_STYLE fontstyle, SGF_FONT_WEIGHT fontweight)
	{
		HRESULT hr = S_OK;

		/* 创建工厂 */
		if (!m_pDWriteFactory)
		{
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(m_pDWriteFactory),
				reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
			);
		}

		if (FAILED(hr))
			return NULL;

		IDWriteTextFormat* writeformat = NULL;
		hr = m_pDWriteFactory->CreateTextFormat(
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
		hr = m_pRenderTarget->CreateSolidColorBrush(
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


	void D2DRender::DestoryTextFormat(TextFormat* format)
	{
		if (format == NULL)
			return;

		D2DTextFormat *d2dformat = (D2DTextFormat *)format;
		D2DSafeRelease(&(d2dformat->textBrush));
		D2DSafeRelease(&(d2dformat->textFormat));
	}


	void D2DRender::SetTextColor(TextFormat* format, DWORD rgb)
	{
		if (!format)
			return;

		D2DTextFormat *d2dformat = (D2DTextFormat *)format;
		if (d2dformat->textBrush)
			d2dformat->textBrush->SetColor(D2D1::ColorF(rgb));
	}


	void D2DRender::DrawText(int x, int y, const TCHAR* text, const TextFormat* format)
	{
		if (format == NULL || text == NULL || _tcsclen(text) <= 0)
			return;

		D2DTextFormat *d2dformat = (D2DTextFormat *)format;
		D2D1_SIZE_F &size = m_pRenderTarget->GetSize();

		m_pRenderTarget->DrawText(
			text,
			_tcsclen(text),
			d2dformat->textFormat,
			D2D1::RectF(x, y, size.width, size.height),
			d2dformat->textBrush
		);
	}
}
