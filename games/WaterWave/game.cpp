#include "interface.h"
#include "sgfrender.h"
#include "sgfwin.h"
#include "sgfeventdispatcher.h"
#include "sgfeventlistenermouse.h"
#include "sgfeventmouse.h"
#include "sgflog.h"
#include <cmath>


#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define RAND_RANAGE(a, b) (a + (rand()%(b-a+1)))

//振幅
int g_nAmplitude[2][WIN_HEIGHT][WIN_WIDTH];
//当前显示位置
int g_nCurPos = 0;

static unsigned char* g_water_pixels = NULL;
static Modules* g_modules = NULL;
static SGF::Bitmap *g_bg = NULL;
static SGF::BitmapPixels g_bg_pixels;


void RippleSpread()
{
	int nNextPos = (g_nCurPos + 1) % 2;

	for (int i = 1; i < WIN_HEIGHT - 1; i++)
	{
		for (int j = 1; j < WIN_WIDTH - 1; j++)
		{
			//振幅计算
			g_nAmplitude[nNextPos][i][j] =
				((g_nAmplitude[g_nCurPos][i - 1][j]
					+ g_nAmplitude[g_nCurPos][i + 1][j]
					+ g_nAmplitude[g_nCurPos][i][j - 1]
					+ g_nAmplitude[g_nCurPos][i][j + 1]) >> 1)
				- g_nAmplitude[nNextPos][i][j];

			//if (g_nAmplitude[nNextPos][i][j] < MIN_VALUE) g_nAmplitude[nNextPos][i][j] = MIN_VALUE;
			//else if (g_nAmplitude[nNextPos][i][j] > MAX_VALUE) g_nAmplitude[nNextPos][i][j] = MAX_VALUE;


			//阻力
			g_nAmplitude[nNextPos][i][j] -= g_nAmplitude[nNextPos][i][j] >> 5;
		}
	}

	g_nCurPos = (g_nCurPos + 1) % 2;
}


//设置起始波动位置
void DropStone(int x, int y, int r, int h)
{
	POINT ptStart;
	SIZE szLen;

	ptStart.x = x - r < 0 ? 0 : x - r;
	ptStart.y = y - r < 0 ? 0 : y - r;
	szLen.cx = x + r >= WIN_WIDTH ? WIN_WIDTH - 1 : x + r;
	szLen.cy = y + r >= WIN_HEIGHT ? WIN_HEIGHT - 1 : y + r;

	for (int i = ptStart.y; i < szLen.cy; i++)
	{
		for (int j = ptStart.x; j < szLen.cx; j++)
		{
			if (((i - y)*(i - y) + (j - x)*(j - x)) < r*r)
				g_nAmplitude[g_nCurPos][i][j] = -h;
		}
	}
}


void RenderRipple()
{
	int offsetX = 0;
	int offsetY = 0;
	int *pBitmap[2] = { (int*)g_bg_pixels.data, (int*)g_water_pixels };
	int width = (int)g_bg_pixels.props.width;
	int height = (int)g_bg_pixels.props.height;
	

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			//计算偏移
			offsetX = g_nAmplitude[g_nCurPos][i][j - 1] - g_nAmplitude[g_nCurPos][i][j + 1];
			offsetY = g_nAmplitude[g_nCurPos][i - 1][j] - g_nAmplitude[g_nCurPos][i + 1][j];
		
			//越界
			if(i+offsetY < 0 || i+offsetY >= height)
				continue;
			if(j+offsetX < 0 || j+offsetX >= width)
				continue;

			//赋值
			pBitmap[1][i*width + j] = pBitmap[0][(i + offsetY)*width + (j + offsetX)];
		}
	}

}

//处理按键输入
static void processInput(float dt)
{
	if(rand() % 10 == 0)
		DropStone(RAND_RANAGE(10,800), RAND_RANAGE(10,600), 
			RAND_RANAGE(5, 10), RAND_RANAGE(32, 150));
}


//初始化游戏
static void initGame(Modules* modules)
{
	if (modules == NULL)
		return;

	g_modules = modules;
	
	if (g_modules && !g_modules->render.expired())
	{
		auto render = g_modules->render.lock();
		render->loadBitmapFromFile(L"./bg.bmp", 0, 0, &g_bg_pixels);

		int nRes = render->createBitmap(g_bg_pixels.props.width, 
			g_bg_pixels.props.height , g_bg_pixels.data, 
			&g_bg_pixels.props, &g_bg);

		if (nRes != 0) {
			return;
		}


		g_water_pixels = new (std::nothrow) unsigned char[g_bg_pixels.size];
		memset(g_water_pixels, 0, g_bg_pixels.size);
	}


	if (g_modules && !g_modules->winApp.expired())
	{
		auto win = g_modules->winApp.lock();
		win->setUpdateFPS(30);
		win->setGameTitle(TEXT("水波"));
	}


	if (g_modules && !g_modules->eventDispatcher.expired())
	{
		auto eventDispather = g_modules->eventDispatcher.lock();

		auto mouseListener = SGF::EventListenerMouse::create();
		mouseListener->onMouseMove = [](SGF::EventMouse* event) {
			SGF::Logger::getInstance()->info("x={}, y={}", 
				event->getCursorX(), event->getCursorY());

			DropStone(event->getCursorX(), event->getCursorY(),
				RAND_RANAGE(5, 10), RAND_RANAGE(32, 150));
		};

		eventDispather->addEventListener(mouseListener);
	}
}


//反初始化游戏
static void uninitGame(Modules* modules)
{
	if (g_water_pixels)
	{
		delete[]g_water_pixels;
		g_water_pixels = NULL;
	}
}


//更新逻辑
static bool updateLogic(float dt)
{
	RippleSpread();

	return true;
}

//绘制帧
static void renderFrame(float dt)
{
	if (g_modules && !g_modules->render.expired())
	{
		auto render = g_modules->render.lock();

	
		RenderRipple();

		if (g_bg)
		{
			render->copyMemToBmp(g_bg, RECT({ 0, 0,
				(LONG)g_bg_pixels.props.width, (LONG)g_bg_pixels.props.height }),
				g_water_pixels, g_bg_pixels.props.width * 4);

			render->drawBitmap(g_bg, 0, 0);
		}
	}
}


#if defined(WATER_WAVE_EXPORTS)
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