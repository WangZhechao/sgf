#pragma once

#include <Windows.h>

struct GameInterface;

namespace SGF
{
	class Render;

	class WinApp
	{
	public:
		WinApp(HINSTANCE);
		~WinApp();

	public:       
		int Initialize(Render* render);
		void Run(GameInterface* game = NULL);


	private:
		int RegisterClassAndCreateWindow();
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	
	private:
		Render* m_pRender;

		LONG m_lWidth;
		LONG m_lHeight;
		DWORD m_dwExStyle;
		DWORD m_dwStyle;
		HINSTANCE m_hInstance;
		HWND m_hWnd;

		bool m_bActive;
		bool m_bSuspend;
		bool m_bWindowed;
	};
}