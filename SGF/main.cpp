/*
 * SGF —— Simple Graphics Frame
 */

#include "sgfwin.h"
#include "sgferr.h"
#include "sgfd2drender.h"
#include "sgfdl.h"
#include "game.h"


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

	GameInterface* pGameInterface = NULL;

	SGF::DLLModule dl;
	if (dl.Open(TEXT("./WaterWave.dll")) < 0)
	{
		MessageBox(NULL, TEXT("加载游戏DLL失败！"), TEXT("错误"), MB_OK);
		return -1;
	}
	else
	{
		GetInterfaceFun fun = (GetInterfaceFun)dl.GetSym("getGameInterface");
		if (fun == NULL)
		{
			dl.Close();
			MessageBox(NULL, TEXT("无法获取接口指针！"), TEXT("错误"), MB_OK);
			return -2;
		}
		else
		{
			pGameInterface = fun();
			if (pGameInterface == NULL)
			{
				dl.Close();
				MessageBox(NULL, TEXT("无法获取游戏接口！"), TEXT("错误"), MB_OK);
				return -3;
			}
		}
	}

	SGF::D2DRender* render = new SGF::D2DRender();
	SGF::WinApp app(hInstance);

	if (app.Initialize(render) == SGF::ERR_NONE)
	{
		app.Run(pGameInterface);
	}

	if (render)
	{
		delete render;
		render = NULL;
	}

	dl.Close();

	return 0;
}