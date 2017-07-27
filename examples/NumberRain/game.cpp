// NumberRain.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "game.h"
#include "sgfrender.h"
#include "rain.h"

const int DEFAULT_FONT_HEIGHT = 12;
const int DEFAULT_FONT_WIDTH = 15;


NumRain* g_numrain = NULL;
float g_fps_time = 0.0f;

static int getFPS()
{
	return DEFAULT_FPS;
}


//��ʼ����Ϸ
static void initGame(SGF::Render* render)
{
	if (render == NULL)
		return;

	SIZE font = { DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT };
	SIZE win; render->GetSize(win);

	g_numrain = new NumRain(win, font);
	if (g_numrain)
	{
		g_numrain->createFont(render);
		g_numrain->genLines();
	}
}


//����ʼ����Ϸ
static void uninitGame(SGF::Render* render)
{
	if (g_numrain)
	{
		g_numrain->destoryFont(render);
		delete g_numrain;
		g_numrain = NULL;
	}
}


//����������
static void processInput(float dt)
{

}


//�����߼�
static bool updateLogic(float dt)
{
	if (g_numrain)
		g_numrain->update();

	return true;
}


//����֡
static void renderFrame(SGF::Render* render, float dt)
{
	if (g_numrain)
	{
		g_numrain->draw(render);
	}
}


#if defined(NUMBERRAIN_EXPORTS)
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