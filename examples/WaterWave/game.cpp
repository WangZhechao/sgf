#include "stdafx.h"
#include "game.h"
#include "sgfrender.h"

static float g_fps_time = 0.0f;
static Bitmap *g_bg = NULL;

static const TCHAR* getName()
{
	static TCHAR* name = L"ˮ��";

	return name;
}

static int getFPS()
{
	return DEFAULT_FPS;
}


//��ʼ����Ϸ
static void initGame(SGF::Render* render)
{
	if (render == NULL)
		return;

	render->LoadBitmapFromFile(L"E:\\github\\sgf\\examples\\WaterWave\\bg.bmp", 0, 0, &g_bg);
}


//����ʼ����Ϸ
static void uninitGame(SGF::Render* render)
{

}


//����������
static void processInput(float dt)
{

}


//�����߼�
static bool updateLogic(float dt)
{

	return true;
}


//����֡
static void renderFrame(SGF::Render* render, float dt)
{
	if (render == NULL)
		return;

	if(g_bg)
		render->DrawBitmap(g_bg, 0, 0);
}


#if defined(WaterWave_EXPORTS)
#define NR_EXTERN __declspec(dllexport)
#else
#define NR_EXTERN __declspec(dllimport)
#endif

//���ؽӿ�

#ifdef __cplusplus
extern "C" {
#endif

	NR_EXTERN GameInterface* getGameInterface()
	{
		static GameInterface inf = {
			getName,
			getFPS,
			initGame,
			processInput,
			updateLogic,
			renderFrame,
			uninitGame
		};

		return &inf;
	}


#ifdef __cplusplus
}
#endif