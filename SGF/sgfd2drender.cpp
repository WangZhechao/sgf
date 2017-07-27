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
		m_pTextFormat(NULL)

	{
		Create();
	}

	D2DRender::~D2DRender()
	{

	}

	//创建
	int D2DRender::Create()
	{
		HRESULT hr = CreateDeviceIndependentResources();
		if (FAILED(hr))
		{
			return ERR_CREATE_D2D_DEVICE_FAIL;
		}

		return hr;
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



			if (m_bFPS)
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


	HRESULT D2DRender::CreateDeviceIndependentResources()
	{
		HRESULT hr = S_OK;

		if(!m_pDirect2dFactory)
			hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);


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
				}


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
	TextFormat* D2DRender::CreateTextFormat()
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


		/* 创建字体格式 */
		static const WCHAR msc_fontName[] = L"Consolas";
		static const FLOAT msc_fontSize = 15;

		IDWriteTextFormat* writeformat = NULL;
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_BOLD,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
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
