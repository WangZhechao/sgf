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
		virtual int Initiate(HWND hwnd);								//��ʼ��
		virtual bool BeginRender();
		virtual void RenderFPS(float dt);								//��ʼ��Ⱦ
		virtual void EndRender();
		virtual void Uninitiate();										//����ʼ��
		virtual void Resize(unsigned int w, unsigned int h);			//������С
		virtual void FocusChange(bool act);								//�ı佹��
		virtual void GetDesktopDpi(float* dpiX, float* dpiY);
		virtual void GetSize(SIZE& size);

		virtual int LoadBitmapFromResource(
			HINSTANCE hinstance,
			PCWSTR resourceName,
			PCWSTR resourceType,
			UINT destinationWidth,
			UINT destinationHeight,
			Bitmap **ppBitmap);


		//�����ļ�λͼ
		virtual int LoadBitmapFromFile(
			PCWSTR filename,
			UINT destinationWidth,
			UINT destinationHeight,
			Bitmap **ppBitmap);


		virtual int LoadBitmapFromFile(
			PCWSTR filename,
			UINT destinationWidth,
			UINT destinationHeight,
			BitmapPixels* pPixels);


		//����ͼ��
		virtual void DrawBitmap(Bitmap* bitmap, int x, int y);

		//��ȡλͼ��ʽ
		virtual BitmapProperties GetBitmapProperties(Bitmap* bitmap);

		//����ͼ��
		virtual int CreateBitmap(unsigned int w, unsigned int h, void* data, BitmapProperties* props, Bitmap **ppBitmap);


		//Copyͼ��
		virtual int CopyBmpToBmp(Bitmap* desc, unsigned int x, unsigned int y,
			Bitmap *src, unsigned int w, unsigned int h);

		virtual int CopyMemToBmp(Bitmap* dst, RECT& rect, void* src, unsigned int pitch);

	public:
		//δ���ƽӿ�
		virtual TextFormat* CreateTextFormat(const TCHAR* fontfamily, int fontsize,
			SGF_FONT_STYLE fontstyle, SGF_FONT_WEIGHT fontweight);
		virtual void DestoryTextFormat(TextFormat* format);

		virtual void SetTextColor(TextFormat* format, DWORD rgb);
		virtual void DrawText(int x, int y, const TCHAR* text, const TextFormat* format);


	private:
		int Create();
		void Destory();
		HRESULT CreateDeviceIndependentResources();
		void DiscardDeviceIndependentResources();

		HRESULT CreateDeviceResources();
		void DiscardDeviceResources();


	private:
		bool m_bFPS;	//�Ƿ���ʾFPS
		unsigned int m_nFPS;
		unsigned int m_nFPSCount;
		float m_lfTotalTime;
		ID2D1SolidColorBrush* m_pBlackBrush;
		IDWriteFactory *m_pDWriteFactory;
		IDWriteTextFormat *m_pTextFormat;
		IWICImagingFactory *m_pWICFactory;
			
		HWND m_hWnd;
		ID2D1Factory* m_pDirect2dFactory;
		ID2D1HwndRenderTarget* m_pRenderTarget;
	};
}