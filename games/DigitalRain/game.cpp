// NumberRain.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "rain.h"
#include "interface.h"
#include "sgfrender.h"
#include "sgfwin.h"

const int DEFAULT_FONT_HEIGHT = 12;
const int DEFAULT_FONT_WIDTH = 15;


static Modules* g_modules = NULL;
static DigitalRain* g_numRain = NULL;


//��ʼ����Ϸ
static void initGame(Modules* modules)
{
	if (modules == NULL)
		return;

	g_modules = modules;

	SIZE font = { DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT };
	SIZE win = { 800, 600 };
	
	if (g_modules && !g_modules->render.expired())
	{
		auto render = g_modules->render.lock();
		render->getSize(win);

		g_numRain = new DigitalRain(win, font);
		if (g_numRain)
		{
			g_numRain->createFont(render.get());
			g_numRain->genLines();
		}
	}


	if (g_modules && !g_modules->winApp.expired())
	{
		auto win = g_modules->winApp.lock();
		win->setUpdateFPS(30);
		win->setGameTitle(TEXT("������"));
	}
}


//����ʼ����Ϸ
static void uninitGame(Modules* modules)
{
	if (g_numRain)
	{
		if (g_modules && !g_modules->render.expired())
		{
			auto render = g_modules->render.lock();
			g_numRain->destoryFont(render.get());
		}
		delete g_numRain;
		g_numRain = NULL;
	}

	g_modules = NULL;
}


//�����߼�
static bool updateLogic(float dt)
{
	if (g_numRain)
		g_numRain->update();

	return true;
}


//����֡
static void renderFrame(float dt)
{
	if (g_modules && !g_modules->render.expired())
	{
		auto render = g_modules->render.lock();
		if (g_numRain)
			g_numRain->draw(render.get());
	}
}


#if defined(NUMBERRAIN_EXPORTS)
#define NR_EXTERN __declspec(dllexport)
#else
#define NR_EXTERN __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

	//���ؽӿ�
	NR_EXTERN GameInterface* getGameInterface()
	{
		static GameInterface inf = {
			initGame,
			updateLogic,
			renderFrame,
			uninitGame
		};

		return &inf;
	}

#ifdef __cplusplus
}
#endif