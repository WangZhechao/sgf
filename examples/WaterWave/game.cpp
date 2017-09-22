#include "stdafx.h"
#include "game.h"
#include "sgfrender.h"
#include <cmath>

static float g_fps_time = 0.0f;


static Bitmap *g_bg = NULL;
static SGF::BitmapPixels g_bg_pixels;
static unsigned char* g_water_pixels = NULL;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define RAND_RANAGE(a, b) (a + (rand()%(b-a+1)))

//���
int g_nAmplitude[2][WIN_HEIGHT][WIN_WIDTH];
//��ǰ��ʾλ��
int g_nCurPos = 0;


void RippleSpread()
{
	int nNextPos = (g_nCurPos + 1) % 2;

	for (int i = 1; i < WIN_HEIGHT - 1; i++)
	{
		for (int j = 1; j < WIN_WIDTH - 1; j++)
		{
			//�������
			g_nAmplitude[nNextPos][i][j] =
				((g_nAmplitude[g_nCurPos][i - 1][j]
					+ g_nAmplitude[g_nCurPos][i + 1][j]
					+ g_nAmplitude[g_nCurPos][i][j - 1]
					+ g_nAmplitude[g_nCurPos][i][j + 1]) >> 1)
				- g_nAmplitude[nNextPos][i][j];

			//if (g_nAmplitude[nNextPos][i][j] < MIN_VALUE) g_nAmplitude[nNextPos][i][j] = MIN_VALUE;
			//else if (g_nAmplitude[nNextPos][i][j] > MAX_VALUE) g_nAmplitude[nNextPos][i][j] = MAX_VALUE;


			//����
			g_nAmplitude[nNextPos][i][j] -= g_nAmplitude[nNextPos][i][j] >> 5;
		}
	}

	g_nCurPos = (g_nCurPos + 1) % 2;
}


//������ʼ����λ��
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
			//����ƫ��
			offsetX = g_nAmplitude[g_nCurPos][i][j - 1] - g_nAmplitude[g_nCurPos][i][j + 1];
			offsetY = g_nAmplitude[g_nCurPos][i - 1][j] - g_nAmplitude[g_nCurPos][i + 1][j];
		
			//Խ��
			if(i+offsetY < 0 || i+offsetY >= height)
				continue;
			if(j+offsetX < 0 || j+offsetX >= width)
				continue;

			//��ֵ
			pBitmap[1][i*width + j] = pBitmap[0][(i + offsetY)*width + (j + offsetX)];
		}
	}

}


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

	render->LoadBitmapFromFile(L"E:\\github\\sgf\\examples\\WaterWave\\bg.bmp", 0, 0, &g_bg_pixels);
	
	int nRes = render->CreateBitmap(g_bg_pixels.props.width, g_bg_pixels.props.height
		, g_bg_pixels.data, &g_bg_pixels.props, &g_bg);

	if (nRes != 0) {
		return;
	}

	g_water_pixels = new unsigned char[g_bg_pixels.size];
	memset(g_water_pixels, 0, g_bg_pixels.size);

	
}


//����ʼ����Ϸ
static void uninitGame(SGF::Render* render)
{
	if (g_water_pixels)
	{
		delete[]g_water_pixels;
		g_water_pixels = NULL;
	}
}


//����������
static void processInput(float dt)
{
	if(rand() % 10 == 0)
		DropStone(RAND_RANAGE(10,800), RAND_RANAGE(10,600), 
			RAND_RANAGE(5, 10), RAND_RANAGE(32, 150));
}


//�����߼�
static bool updateLogic(float dt)
{
	RippleSpread();

	return true;
}


//����֡
static void renderFrame(SGF::Render* render, float dt)
{
	if (render == NULL)
		return;


	RenderRipple();


	if (g_bg)
	{
		render->CopyMemToBmp(g_bg, RECT({ 0, 0,
			(LONG)g_bg_pixels.props.width, (LONG)g_bg_pixels.props.height }),
			g_water_pixels, g_bg_pixels.props.width * 4);

		render->DrawBitmap(g_bg, 0, 0);
	}
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