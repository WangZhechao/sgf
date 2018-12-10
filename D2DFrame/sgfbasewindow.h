#pragma once

#include <Windows.h>
#include <memory>
#include "sgferr.h"

namespace SGF
{
	class GameApp;

	class BaseWindow
	{
	public:
		BaseWindow(const std::shared_ptr<GameApp>& app);
		virtual ~BaseWindow();

		BaseWindow(const BaseWindow&) = delete;
		BaseWindow& operator=(const BaseWindow&) = delete;


	public:
		const HWND& getHWND() const;
		void setGameTitle(const TCHAR* title);
		void setWindowSize(const SIZE& size);

	public:
		virtual ERR_TYPE initiate();
		virtual void uninitiate();
		virtual void run(int nCmdShow = SW_SHOWNORMAL);


	protected:
		virtual ERR_TYPE registerWinClass();
		virtual void unregisterWinClass();
		virtual ERR_TYPE createWindow();
		virtual void destroyWindow();
		virtual SIZE getHightDpiWindowSize(const SIZE& size);
		virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	

	private:
		std::shared_ptr<GameApp> _baseApp;
		SIZE _winSize;
		DWORD _styleEx;
		DWORD _style;
		HINSTANCE _instance;
		HWND _wnd;
		bool _active;
	};
}