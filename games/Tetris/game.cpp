#include "interface.h"


//��ʼ����Ϸ
static void initGame(Modules* modules)
{

}


//����ʼ����Ϸ
static void uninitGame(Modules* modules)
{

}


//�����߼�
static bool updateLogic(float dt)
{

	return true;
}

//����֡
static void renderFrame(float dt)
{

}


#if defined(TETRIS_EXPORTS)
#define NR_EXTERN __declspec(dllexport)
#else
#define NR_EXTERN __declspec(dllimport)
#endif


//���ؽӿ�
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