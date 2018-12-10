#include "sgfbasewindow.h"
#include "sgfgameapp.h"
#include "sgfd2drender.h"
#include "sgfeventmouse.h"
#include "sgfeventkeyboard.h"
#include "sgfeventdispatcher.h"
#include <math.h>
#include <tchar.h>
#include <assert.h>


#define APP_CLASS_NAME TEXT("WinAppClass")
#define APP_WIN_NAME TEXT("Direct2D")

namespace SGF
{
	const int DEFAULT_WINDOW_WIDTH = 800;
	const int DEFAULT_WINDOW_HEIGHT = 600;
	const float DEFAULT_SYSTEM_DPI = 96.0f;
	const int DEFAULT_FPS = 30;

	BaseWindow::BaseWindow(const std::shared_ptr<GameApp>& app)
		: _baseApp(app),
		_instance(NULL),
		_styleEx(WS_EX_APPWINDOW),
		_style(WS_OVERLAPPEDWINDOW),
		_wnd(NULL),
		_active(true)
	{
		memset(&_winSize, 0, sizeof(_winSize));
		_winSize.cx = DEFAULT_WINDOW_WIDTH;
		_winSize.cy = DEFAULT_WINDOW_HEIGHT;

		_instance = _baseApp->getHINSTANCE();
	}


	BaseWindow::~BaseWindow()
	{

	}


	void BaseWindow::uninitiate()
	{
		destroyWindow();
		unregisterWinClass();
	}


	ERR_TYPE BaseWindow::initiate()
	{
		ERR_TYPE ret = registerWinClass();
		if (ret != ERR_NONE) {
			uninitiate();
			return ret;
		}

		ret = createWindow();
		if (ret != ERR_NONE) {
			uninitiate();
			return ret;
		}

		return ret;
	}


	//运行
	void BaseWindow::run(int nCmdShow)
	{
		//依赖 Render
		assert(_baseApp && _baseApp->getRender());
		std::shared_ptr<D2DRender> render = _baseApp->getRender();

		//显示更新窗口 
		ShowWindow(_wnd, nCmdShow);
		UpdateWindow(_wnd);

		MSG nMsg = { 0 };
		bool bQuit = false;
		float elapsed = 0.0f;
		LARGE_INTEGER nFrequency, nPrevTime, nCurrTime, nElapsedCounter;
		float lag = 0.0f;
		float frameMS = 1000.0f / DEFAULT_FPS;

		//TODO: 游戏初始化
		if (_baseApp) {
			int ret = _baseApp->gameInitiate();
			if (ret != 0) {
				_baseApp->gameUninitiate();
				return;
			}
		}


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
				if (_active)
				{
					//游戏更新
					while (lag >= frameMS)
					{
						//TODO: 更新逻辑
						if (_baseApp) {
							_baseApp->gameUpdate(frameMS);
						}

						lag -= frameMS;
					}


					if (render->beginRender())
					{
						//TODO: 游戏渲染
						if (_baseApp) {
							_baseApp->gameRender(lag);
						}

						render->renderFPS(elapsed);
						render->endRender();
					}
				}
			}
		}


		//TODO: 游戏退出
		if (_baseApp) {
			_baseApp->gameUninitiate();
		}

		destroyWindow();
	}

	const HWND& BaseWindow::getHWND() const
	{
		return _wnd;
	}


	void BaseWindow::setGameTitle(const TCHAR *title)
	{
		TCHAR text[512] = { 0 };
		
		if (title)
		{
			assert(_tcsclen(title) < 256);

			_stprintf(text, TEXT("%s"), title);

			if (_wnd)
				::SetWindowText(_wnd, title);
		}
	}

	void BaseWindow::setWindowSize(const SIZE& size)
	{
		_winSize = size;
		SIZE hdpi = getHightDpiWindowSize(_winSize);

		if (_wnd)
		{
			const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
			const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

			::SetWindowPos(_wnd, NULL, (SCREEN_WIDTH - hdpi.cx) * 0.5f,
				(SCREEN_HEIGHT - hdpi.cy) * 0.5f, hdpi.cx,
				hdpi.cy, SWP_NOZORDER);
		}
	}

	SIZE BaseWindow::getHightDpiWindowSize(const SIZE& size)
	{
		//获取当前dpi
		float dpiX = DEFAULT_SYSTEM_DPI;
		float dpiY = DEFAULT_SYSTEM_DPI;

		//依赖 Render
		assert(_baseApp && _baseApp->getRender());

		_baseApp->getRender()->getDesktopDpi(&dpiX, &dpiY);

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


	void BaseWindow::unregisterWinClass()
	{
		UnregisterClass(APP_CLASS_NAME, _instance);
	}


	//注册窗口类
	ERR_TYPE BaseWindow::registerWinClass()
	{
		//定义填充窗口 
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_VREDRAW | CS_HREDRAW;
		wcex.lpfnWndProc = BaseWindow::wndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
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

	void BaseWindow::destroyWindow()
	{
		if (_wnd) {
			DestroyWindow(_wnd);
			_wnd = NULL;
		}
	}

	//注册窗口类，创建窗口
	ERR_TYPE BaseWindow::createWindow()
	{
		//获取真实宽高
		_winSize = getHightDpiWindowSize(_winSize);


		//获取当前窗口样式调整后的矩形大小
		RECT winRect = { 0, 0, _winSize.cx, _winSize.cy };
		if (!AdjustWindowRectEx(&winRect, _style, FALSE, _styleEx))
		{
			return ERR_ADJUST_WIN_FAIL;
		}


		const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
		const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);


		//创建窗口
		if (!(_wnd = CreateWindowEx(_styleEx, APP_CLASS_NAME, APP_WIN_NAME, _style,
			(SCREEN_WIDTH - _winSize.cx) / 2, (SCREEN_HEIGHT - _winSize.cy) / 2,
			_winSize.cx, _winSize.cy, NULL, NULL, _instance, this)))
		{
			DWORD err = GetLastError();

			return ERR_CREATE_WIN_FAIL;
		}


		return ERR_NONE;
	}


	//回调函数
	LRESULT CALLBACK BaseWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		BaseWindow *pApp = NULL;
		static bool inited = false;

		if (message == WM_NCCREATE)
		{
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			pApp = (BaseWindow *)pcs->lpCreateParams;

			LONG_PTR ret = ::SetWindowLongPtr(
				hwnd,
				GWLP_USERDATA,
				(LONG_PTR)pApp
			);

			inited = true;
			pApp->_wnd = hwnd;
		}
		else if(inited)
		{
			pApp = reinterpret_cast<BaseWindow *>(::GetWindowLongPtr( hwnd, GWLP_USERDATA ));
		}


		if (pApp)
		{
			return pApp->handleMessage(message, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}


	LRESULT BaseWindow::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//依赖 Render
		assert(_baseApp && _baseApp->getRender());

		switch (uMsg)
		{
		case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			_baseApp->getRender()->resize(width, height);

			return 0;
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

		case WM_ACTIVATE:
		{
			bool bActivating = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);
			if (_active != bActivating) {

				_baseApp->getRender()->focusChange(bActivating);

				_active = bActivating;
			}
			return 0;
		}
		break;

		case WM_DISPLAYCHANGE:
		{
			InvalidateRect(_wnd, NULL, FALSE);
			return 0;
		}
		break;


		case WM_KEYDOWN:
		{
			EventKeyboard::KeyCode code = EventKeyboard::KeyCode::KEY_NONE;

			switch (wParam)
			{
			case VK_ESCAPE: code = EventKeyboard::KeyCode::KEY_ESCAPE; break;
			case VK_SPACE: code = EventKeyboard::KeyCode::KEY_SPACE; break;
			case VK_UP: code = EventKeyboard::KeyCode::KEY_UP; break;
			case VK_LEFT: code = EventKeyboard::KeyCode::KEY_LEFT; break;
			case VK_RIGHT: code = EventKeyboard::KeyCode::KEY_RIGHT; break;
			case VK_DOWN: code = EventKeyboard::KeyCode::KEY_DOWN; break;
			case VK_F1: code = EventKeyboard::KeyCode::KEY_F1; break;
			}

			EventKeyboard event(code, true);
			_baseApp->getEventDispatcher()->dispatchEvent(&event);
		}
		break;


		case WM_KEYUP:
		{
			EventKeyboard::KeyCode code = EventKeyboard::KeyCode::KEY_NONE;

			switch (wParam)
			{
			case VK_ESCAPE: code = EventKeyboard::KeyCode::KEY_ESCAPE; break;
			case VK_SPACE: code = EventKeyboard::KeyCode::KEY_SPACE; break;
			case VK_UP: code = EventKeyboard::KeyCode::KEY_UP; break;
			case VK_LEFT: code = EventKeyboard::KeyCode::KEY_LEFT; break;
			case VK_RIGHT: code = EventKeyboard::KeyCode::KEY_RIGHT; break;
			case VK_DOWN: code = EventKeyboard::KeyCode::KEY_DOWN; break;
			case VK_F1: code = EventKeyboard::KeyCode::KEY_F1; break;
			}

			EventKeyboard event(code, false);
			_baseApp->getEventDispatcher()->dispatchEvent(&event);
		}
		break;


		case WM_LBUTTONDOWN:
		{
			assert(_baseApp && _baseApp->getEventDispatcher());

			int buttons = EventMouse::MouseButton::BUTTON_LEFT;

			//按下事件
			EventMouse downEvt(EventMouse::MouseEventType::MOUSE_DOWN);
			downEvt.setMouseButton(buttons);
			downEvt.setCursorPosition(LOWORD(lParam), HIWORD(lParam));
			_baseApp->getEventDispatcher()->dispatchEvent(&downEvt);

			//点击事件
			EventMouse clickEvt(EventMouse::MouseEventType::MOUSE_CLICK);
			clickEvt.setMouseButton(buttons);
			clickEvt.setCursorPosition(LOWORD(lParam), HIWORD(lParam));
			_baseApp->getEventDispatcher()->dispatchEvent(&clickEvt);
		}
		break;


		case WM_LBUTTONUP:
		{
			assert(_baseApp && _baseApp->getEventDispatcher());

			int buttons = EventMouse::MouseButton::BUTTON_LEFT;

			//弹起事件
			EventMouse upEvt(EventMouse::MouseEventType::MOUSE_UP);
			upEvt.setMouseButton(buttons);
			upEvt.setCursorPosition(LOWORD(lParam), HIWORD(lParam));
			_baseApp->getEventDispatcher()->dispatchEvent(&upEvt);
		}
		break;


		case WM_RBUTTONDOWN:
		{
			assert(_baseApp && _baseApp->getEventDispatcher());

			int buttons = EventMouse::MouseButton::BUTTON_RIGHT;

			//按下事件
			EventMouse downEvt(EventMouse::MouseEventType::MOUSE_DOWN);
			downEvt.setMouseButton(buttons);
			downEvt.setCursorPosition(LOWORD(lParam), HIWORD(lParam));
			_baseApp->getEventDispatcher()->dispatchEvent(&downEvt);

			//点击事件
			EventMouse clickEvt(EventMouse::MouseEventType::MOUSE_CLICK);
			clickEvt.setMouseButton(buttons);
			clickEvt.setCursorPosition(LOWORD(lParam), HIWORD(lParam));
			_baseApp->getEventDispatcher()->dispatchEvent(&clickEvt);
		}
		break;


		case WM_RBUTTONUP:
		{
			assert(_baseApp && _baseApp->getEventDispatcher());

			int buttons = EventMouse::MouseButton::BUTTON_RIGHT;

			//弹起事件
			EventMouse upEvt(EventMouse::MouseEventType::MOUSE_UP);
			upEvt.setMouseButton(buttons);
			upEvt.setCursorPosition(LOWORD(lParam), HIWORD(lParam));
			_baseApp->getEventDispatcher()->dispatchEvent(&upEvt);
		}
		break;


		case WM_MOUSEMOVE:
		{
			//依赖input
			assert(_baseApp && _baseApp->getEventDispatcher());

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
			_baseApp->getEventDispatcher()->dispatchEvent(&event);
		}
		break;


		default:
			return DefWindowProc(_wnd, uMsg, wParam, lParam);
		}

		return 1;
	}
}