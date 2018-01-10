#include "interface.h"


//初始化游戏
static void initGame(Modules* modules)
{

}


//反初始化游戏
static void uninitGame(Modules* modules)
{

}


//更新逻辑
static bool updateLogic(float dt)
{

	return true;
}

//绘制帧
static void renderFrame(float dt)
{

}


#if defined(TETRIS_EXPORTS)
#define NR_EXTERN __declspec(dllexport)
#else
#define NR_EXTERN __declspec(dllimport)
#endif


//返回接口
#ifdef __cplusplus
extern "C" {
#endif

	//返回接口
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