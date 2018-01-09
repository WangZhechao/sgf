/*
 * SGF —— Simple Graphics Frame
 * author: W_Z_C
 */

#define WIN32_LEAN_AND_MEAN //一个宏，用来告诉编译器你使用WIN32，不用也行！ 

#include <Windows.h>
#include <WindowsX.h>

#include "resource.h"
#include "interface.h"
#include "sgferr.h"
#include "sgfdl.h"
#include "sgfwin.h"
#include "sgfd2drender.h"
#include "sgfeventdispatcher.h"
#include "sgflog.h"
#include <tchar.h>

#define DEFAULT_GAME_DIR (TEXT("./games"))

static TCHAR g_curr_load_dll[512] = { 0 };
static SGF::DLLModule g_dl;

int addGameList(HWND hctl, const TCHAR* dir)
{
	WIN32_FIND_DATA findData = { 0 };

	TCHAR search[512] = { 0 };
	_stprintf(search, TEXT("%s/*.dll"), dir);

	HANDLE handle = FindFirstFile(search, &findData);
	if (handle == NULL) {
		return 0;
	}

	int count = 0;
	do
	{
		ComboBox_AddString(hctl, findData.cFileName);

		count++;

	} while (FindNextFile(handle, &findData));

	FindClose(handle);

	return count;
}


GameInterface* loadDLLAndGetInterface(TCHAR* filename)
{
	if (g_dl.open(filename) < 0)
	{
		return NULL;
	}

	GetInterfaceFun fun = (GetInterfaceFun)g_dl.getSym("getGameInterface");
	if (fun == NULL)
	{
		g_dl.close();
		return NULL;
	}
	else
	{
		GameInterface* inf = fun();
		if (inf == NULL)
		{
			g_dl.close();
			return NULL;
		}

		return inf;
	}

	return NULL;
}


LRESULT CALLBACK dlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		{
			int count = addGameList(GetDlgItem(hWndDlg, IDC_CMB_GAMELIST), DEFAULT_GAME_DIR);
			if (count > 0) {
				ComboBox_SetCurSel(GetDlgItem(hWndDlg, IDC_CMB_GAMELIST), 0);
			}

			ComboBox_AddString(GetDlgItem(hWndDlg, IDC_CMB_RENDER), TEXT("D2D引擎"));
			ComboBox_SetCurSel(GetDlgItem(hWndDlg, IDC_CMB_RENDER), 0);
		}
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWndDlg, -1);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_BTN_OPEN:
			{
				TCHAR fileName[512] = { 0 };
				int len = ComboBox_GetText(GetDlgItem(hWndDlg, IDC_CMB_GAMELIST), fileName, 512);
				if (len > 0) {
					_stprintf(g_curr_load_dll, TEXT("%s/%s"), DEFAULT_GAME_DIR, fileName);
				} else {
					MessageBox(hWndDlg, TEXT("无法获取DLL名称!"), TEXT("错误"), MB_OK);
					EndDialog(hWndDlg, -1);
				}

				EndDialog(hWndDlg, 0);
			}
			return TRUE;

		case IDC_BTN_CLOSE:
			EndDialog(hWndDlg, -1);
			return TRUE;
		}
		break;
	}

	return FALSE;
}


int WINAPI WinMain(
	HINSTANCE hInstance/* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
)
{
	// Use HeapSetInformation to specify that the process should
	// terminate if the heap manager detects an error in any heap used
	// by the process.
	// The return value is ignored, because we want to continue running in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	
	std::shared_ptr<SGF::WinApp> app = std::make_shared<SGF::WinApp>(hInstance);
	std::shared_ptr<SGF::Render> render = std::make_shared<SGF::D2DRender>();
	std::shared_ptr<SGF::EventDispatcher> dispatcher = std::make_shared<SGF::EventDispatcher>();
	

	INT_PTR ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_GAMELIST),
		NULL, reinterpret_cast<DLGPROC>(dlgProc));
	if (ret != 0) {
		return -1;
	}


	Modules modules = {
		render,
		dispatcher,
		app
	};

	if (app->initialize(&modules) == SGF::ERR_NONE)
	{
		//加载游戏
		GameInterface* inf = loadDLLAndGetInterface(g_curr_load_dll);
		if (inf == NULL) {
			MessageBox(app->getWnd(), TEXT("DLL加载失败！"), TEXT("错误"), MB_OK);
			return -2;
		}

		app->run(inf);
	}


	g_dl.close();

	return 0;
}