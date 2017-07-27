#include <math.h>
#include "sgfwin.h"
#include "sgferr.h"
#include "sgfrender.h"
#include "game.h"

#define APP_CLASS_NAME TEXT("SGFWinAppClass")
#define APP_WIN_NAME TEXT("SGFWinApp")


namespace SGF
{
	const float DEFAULT_SYSTEM_DPI = 96.0f;

	WinApp::WinApp(HINSTANCE hinstance) :
		m_pRender(NULL),
		m_lWidth(800),
		m_lHeight(600),
		m_dwExStyle(WS_EX_APPWINDOW),
		m_dwStyle(WS_OVERLAPPEDWINDOW),
		m_hInstance(hinstance),
		m_hWnd(NULL),
		m_bActive(true),
		m_bSuspend(false),
		m_bWindowed(true)
	{

	}


	WinApp::~WinApp()
	{
	}


	//注册窗口类，创建窗口
	int WinApp::RegisterClassAndCreateWindow()
	{
		//定义填充窗口 
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_VREDRAW | CS_HREDRAW;
		wcex.lpfnWndProc = WinApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = m_hInstance;
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


		//获取当前dpi
		float dpiX = DEFAULT_SYSTEM_DPI;
		float dpiY = DEFAULT_SYSTEM_DPI;
		if (m_pRender)
			m_pRender->GetDesktopDpi(&dpiX, &dpiY);


		//获取当前窗口样式调整后的矩形大小
		RECT winRect = { 0, 0, m_lWidth, m_lHeight };
		if (!AdjustWindowRectEx(&winRect, m_dwStyle, FALSE, m_dwExStyle))
		{
			UnregisterClass(APP_CLASS_NAME, m_hInstance);

			return ERR_ADJUST_WIN_FAIL;
		}


		//获取调整后窗口宽高
		const UINT WIN_WIDTH = static_cast<UINT>(ceil((winRect.right - winRect.left) * dpiX / DEFAULT_SYSTEM_DPI));
		const UINT WIN_HEIGHT = static_cast<UINT>(ceil((winRect.bottom - winRect.top) * dpiY / DEFAULT_SYSTEM_DPI));
		const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
		const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);


		//创建窗口
		if (!(m_hWnd = CreateWindowEx(m_dwExStyle, APP_CLASS_NAME, APP_WIN_NAME, m_dwStyle,
			(SCREEN_WIDTH - WIN_WIDTH) / 2, (SCREEN_HEIGHT - WIN_HEIGHT) / 2, 
			WIN_WIDTH, WIN_HEIGHT, NULL, NULL, m_hInstance, this)))
		{
			UnregisterClass(APP_CLASS_NAME, m_hInstance);

			return ERR_CREATE_WIN_FAIL;
		}


		//显示更新窗口 
		ShowWindow(m_hWnd, SW_SHOWNORMAL);
		UpdateWindow(m_hWnd);

		return ERR_NONE;
	}



	//回调函数
	LRESULT CALLBACK WinApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

			if (pApp->m_pRender)
				pApp->m_pRender->Initiate(hwnd);


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
						
						if(pApp->m_pRender)
							pApp->m_pRender->Resize(width, height);

						result = 0;
						wasHandled = true;
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
						if (pApp->m_bActive != bActivating) {

							if (pApp->m_pRender)
								pApp->m_pRender->FocusChange(bActivating);

							pApp->m_bActive = bActivating;
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


	//初始化
	int WinApp::Initialize(Render* render)
	{
		m_pRender = render;

		return RegisterClassAndCreateWindow();
	}


	//运行
	void WinApp::Run(GameInterface* game)
	{
		MSG nMsg = { 0 };
		bool bQuit = false;
		float elapsed = 0.0f;
		LARGE_INTEGER nFrequency, nPrevTime, nCurrTime, nElapsedCounter;
		float lag = 0.0f;
		float frameMS = 1000.0f / DEFAULT_FPS;


		//游戏初始化
		if (game && game->init)
			game->init(m_pRender);


		//获取每帧更新时间
		if (game && game->getfps)
		{
			int fps = game->getfps();
			if (fps <= 0)
				fps = DEFAULT_FPS;

			frameMS = 1000.0f / fps;
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
				if (m_bActive && (!m_bSuspend))
				{
					//游戏输入
					if (game &&game->input)
						game->input(elapsed);


					//游戏更新
					while (lag >= frameMS)
					{
						//更新逻辑
						if (game && game->update)
							game->update(frameMS);


						lag -= frameMS;
					}


					//游戏渲染
					if (m_pRender && m_pRender->BeginRender())
					{
						if (game && game->render)
						{
							game->render(m_pRender, lag);
						}

						m_pRender->RenderFPS(elapsed);
						m_pRender->EndRender();
					}
				}

				//小游戏，降低点cpu
				Sleep(1);
			}
		}

		if (game && game->uninit)
			game->uninit(m_pRender);

		if (m_pRender)
			m_pRender->Uninitiate();


		if (m_hWnd) {
			DestroyWindow(m_hWnd);
			m_hWnd = 0;
		}

		UnregisterClass(APP_CLASS_NAME, m_hInstance);
	}
}