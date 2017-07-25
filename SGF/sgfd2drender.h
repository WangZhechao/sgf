#pragma once

#include <Windows.h>
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1helper.h>

#include "sgfrender.h"

namespace SGF
{

	class D2DRender : public Render
	{
	public:
		D2DRender();
		virtual ~D2DRender();


	public:
		virtual int Initiate(HWND hwnd);		//初始化
		virtual void RenderFrame(float dt);			//开始渲染
		virtual void Uninitiate();		//反初始化
		virtual void Resize(unsigned int w, unsigned int h);			//调整大小
		virtual void FocusChange(bool act);								//改变焦点
		virtual void GetDesktopDpi(float* dpiX, float* dpiY);


	private:
		int Create();
		HRESULT CreateDeviceIndependentResources();
		void DiscardDeviceIndependentResources();

		HRESULT CreateDeviceResources();
		void DiscardDeviceResources();


	private:
		bool m_bFPS;	//是否显示FPS
		unsigned int m_nFPS;
		unsigned int m_nFPSCount;
		float m_lfTotalTime;
		ID2D1SolidColorBrush* m_pBlackBrush;
		IDWriteFactory *m_pDWriteFactory;
		IDWriteTextFormat *m_pTextFormat;
			
		HWND m_hWnd;
		ID2D1Factory* m_pDirect2dFactory;
		ID2D1HwndRenderTarget* m_pRenderTarget;
	};
}