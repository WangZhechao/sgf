#include "sgfwin.h"
#include "sgferr.h"
#include "interface.h"
#include "sgfrender.h"
#include "sgfeventmouse.h"
#include "sgfeventdispatcher.h"
#include <math.h>
#include <tchar.h>
#include <assert.h>

#define APP_CLASS_NAME TEXT("SGFWinAppClass")
#define APP_WIN_NAME TEXT("SGFWinApp")

namespace SGF
{

const float DEFAULT_SYSTEM_DPI = 96.0f;
const int DEFAULT_FPS = 30;

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

WinApp::WinApp(HINSTANCE hinstance) :
	_modules(NULL),
	_styleEx(WS_EX_APPWINDOW),
	_style(WS_OVERLAPPEDWINDOW),
	_instance(hinstance),
	_wnd(NULL),
	_active(true),
	_suspend(false),
	_windowed(true),
	_fps(DEFAULT_FPS)
{
	memset(&_winSize, 0, sizeof(_winSize));
	_winSize.cx = DEFAULT_WINDOW_WIDTH;
	_winSize.cy = DEFAULT_WINDOW_HEIGHT;

	memset(_title, 0, sizeof(_title));
	_stprintf(_title, TEXT("%s"), APP_WIN_NAME);

	int reg = registerWinClass();
	assert(reg == ERR_NONE);
}


WinApp::~WinApp()
{
	UnregisterClass(APP_CLASS_NAME, _instance);
}


SIZE WinApp::getHightDpiWindowSize(const SIZE& size)
{
	//获取当前dpi
	float dpiX = DEFAULT_SYSTEM_DPI;
	float dpiY = DEFAULT_SYSTEM_DPI;

	if (_modules && !_modules->render.expired())
	{
		auto render = _modules->render.lock();
		render->getDesktopDpi(&dpiX, &dpiY);
	}

	SIZE adjustSize = { 0 };


	//获取调整后窗口宽高
	adjustSize.cx = static_cast<UINT>(ceil((size.cx) * dpiX / DEFAULT_SYSTEM_DPI));
	adjustSize.cy = static_cast<UINT>(ceil((size.cy) * dpiY / DEFAULT_SYSTEM_DPI));


	//获取当前窗口样式调整后的矩形大小
	RECT winRect = { 0, 0, adjustSize.cx, adjustSize.cy };
	if (!AdjustWindowRectEx(&winRect, _style, FALSE, _styleEx))
	{
		return adjustSize;
	}
	

	adjustSize.cx = winRect.right - winRect.left;
	adjustSize.cy = winRect.bottom - winRect.top;


	return adjustSize;
}

//注册窗口类
int WinApp::registerWinClass()
{
	//定义填充窗口 
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = WinApp::wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = _instance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = APP_CLASS_NAME;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


	//注册窗口类 
	if (!RegisterClassEx(&wcex))
	{
		return ERR_REGISTER_CLASS_FAIL;
	}

	return ERR_NONE;
}



//注册窗口类，创建窗口
int WinApp::createWindow()
{
	//获取真实宽高
	_winSize = getHightDpiWindowSize(_winSize);


	//获取当前窗口样式调整后的矩形大小
	RECT winRect = { 0, 0, _winSize.cx, _winSize.cy };
	if (!AdjustWindowRectEx(&winRect, _style, FALSE, _styleEx))
	{
		UnregisterClass(APP_CLASS_NAME, _instance);

		return ERR_ADJUST_WIN_FAIL;
	}

	
	const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
	const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);


	//创建窗口
	if (!(_wnd = CreateWindowEx(_styleEx, APP_CLASS_NAME, APP_WIN_NAME, _style,
		(SCREEN_WIDTH - _winSize.cx) / 2, (SCREEN_HEIGHT - _winSize.cy) / 2,
		_winSize.cx, _winSize.cy, NULL, NULL, _instance, this)))
	{
		UnregisterClass(APP_CLASS_NAME, _instance);

		return ERR_CREATE_WIN_FAIL;
	}

	return ERR_NONE;
}

//回调函数
LRESULT CALLBACK WinApp::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		WinApp *pApp = (WinApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pApp)
		);

		if (pApp->_modules && !pApp->_modules->render.expired())
		{
			auto render = pApp->_modules->render.lock();
			render->initiate(hwnd);
		}

		result = 1;
	}
	else
	{
		WinApp *pApp = reinterpret_cast<WinApp *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			)));


		bool wasHandled = false;
		if (pApp)
		{
			switch (message)
			{

			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);

				if (pApp->_modules && !pApp->_modules->render.expired())
				{
					auto render = pApp->_modules->render.lock();
					render->resize(width, height);
				}

				result = 0;
				wasHandled = true;
			}
			break;

			case WM_MOUSEMOVE:
			{
				EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);

				int buttons = EventMouse::MouseButton::BUTTON_UNSET;
				if (wParam & MK_LBUTTON) {
					buttons |= EventMouse::MouseButton::BUTTON_LEFT;
				}
				if (wParam & MK_MBUTTON) {
					buttons |= EventMouse::MouseButton::BUTTON_MIDDLE;
				}
				if (wParam & MK_RBUTTON) {
					buttons |= EventMouse::MouseButton::BUTTON_RIGHT;
				}

				event.setMouseButton(buttons);
				event.setCursorPosition(LOWORD(lParam), HIWORD(lParam));

				if (pApp->_modules && !pApp->_modules->eventDispatcher.expired())
				{
					auto eventDispatcher = pApp->_modules->eventDispatcher.lock();
					eventDispatcher->dispatchEvent(&event);
				}
			}
			break;

			//case WM_PAINT:
			//{
			//	//窗口模式，渲染
			//	if (pApp->m_bWindowed && (!pApp->m_bSuspend) && pApp->m_pRender)
			//	{
			//		pApp->m_pRender->RenderFrame(0);
			//	}
			//	result = 0;
			//	wasHandled = true;
			//}
			//break;

			case WM_ACTIVATE:
			{
				bool bActivating = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);
				if (pApp->_active != bActivating) {

					if (pApp->_modules && !pApp->_modules->render.expired())
					{
						auto render = pApp->_modules->render.lock();
						render->focusChange(bActivating);
					}

					pApp->_active = bActivating;
				}

				result = 0;
				wasHandled = true;
			}
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
				result = 0;
				wasHandled = true;
			}
			break;


			case WM_DESTROY:
			{
				PostQuitMessage(0);
				result = 1;
				wasHandled = true;
			}
			break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}


void WinApp::setGameTitle(const TCHAR *title)
{
	if (title) 
	{
		_stprintf(_title, TEXT("%s"), title);

		if(_wnd)
			::SetWindowText(_wnd, title);
	}	
}


void WinApp::setWindowSize(const SIZE& size)
{
	_winSize = size;
	SIZE hdpi = getHightDpiWindowSize(_winSize);

	if(_wnd)
		::SetWindowPos(_wnd, NULL, 0, 0, hdpi.cx, hdpi.cy, SWP_NOZORDER | SWP_NOMOVE);
}


void WinApp::setUpdateFPS(int fps)
{
	if (fps <= 0)
		_fps = DEFAULT_FPS;
	else
		_fps = fps;
}


HWND WinApp::getWnd()
{
	return _wnd;
}

//初始化
int WinApp::initialize(Modules* modules)
{
	_modules = modules;

	return createWindow();
}


//运行
void WinApp::run(GameInterface* game)
{
	//显示更新窗口 
	ShowWindow(_wnd, SW_SHOWNORMAL);
	UpdateWindow(_wnd);


	MSG nMsg = { 0 };
	bool bQuit = false;
	float elapsed = 0.0f;
	LARGE_INTEGER nFrequency, nPrevTime, nCurrTime, nElapsedCounter;
	float lag = 0.0f;
	float frameMS = 1000.0f / _fps;


	//游戏初始化
	if (game && game->init)
		game->init(_modules);


	QueryPerformanceFrequency(&nFrequency);
	QueryPerformanceCounter(&nPrevTime);



	while (!bQuit)
	{
		//获取消息
		if (PeekMessage(&nMsg, NULL, 0, 0, PM_REMOVE))
		{
			if (nMsg.message == WM_QUIT)
			{
				bQuit = true;
			}
			else
			{
				TranslateMessage(&nMsg);
				DispatchMessage(&nMsg);
			}
		}
		else
		{
			QueryPerformanceCounter(&nCurrTime);
			nElapsedCounter.QuadPart = nCurrTime.QuadPart - nPrevTime.QuadPart;
			nPrevTime.QuadPart = nCurrTime.QuadPart;

			elapsed = nElapsedCounter.QuadPart * 1000.0f / nFrequency.QuadPart;
			lag += elapsed;

			//程序激活并且没有主动挂起
			if (_active && (!_suspend))
			{
				//游戏更新
				while (lag >= frameMS)
				{
					//更新逻辑
					if (game && game->update)
						game->update(frameMS);


					lag -= frameMS;
				}


				//游戏渲染
				if (_modules && !_modules->render.expired())
				{
					auto render = _modules->render.lock();
					if (render && render->beginRender())
					{
						if (game && game->render)
						{
							game->render(lag);
						}

						render->renderFPS(elapsed);
						render->endRender();
					}
				}
			}

			//小游戏，降低点cpu
			Sleep(1);
		}
	}


	if (game && game->uninit)
	{
		game->uninit(_modules);
	}


	if (_wnd) {
		DestroyWindow(_wnd);
		_wnd = 0;
	}
}




}