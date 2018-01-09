#pragma once

#include <Windows.h>
#include "interface.h"

namespace SGF
{
class WinApp
{
public:
	WinApp(HINSTANCE);
	virtual ~WinApp();

	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;


public:
	int initialize(Modules* modules);
	void run(GameInterface* game);

public:
	void setDefFPS(int fps);
	void setGameTitle(const TCHAR* title);
	void setWindowSize(const SIZE& size);
	HWND getWnd();

private:
	SIZE getHightDpiWindowSize(const SIZE& size);
	int registerClassAndCreateWindow();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	
private:
	Modules* _modules;
	TCHAR _title[512];
	SIZE _winSize;
	DWORD _styleEx;
	DWORD _style;
	HINSTANCE _instance;
	HWND _wnd;
	int _fps;

	bool _active;
	bool _suspend;
	bool _windowed;
};
}