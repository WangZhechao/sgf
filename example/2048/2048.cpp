#include "2048.h"
#include "sgfbasewindow.h"
#include "sgfd2drender.h"
#include "sgfeventdispatcher.h"
#include "sgfeventlistenermouse.h"
#include "sgfeventmouse.h"
#include "sgfeventlistenerkeyboard.h"
#include "sgfeventkeyboard.h"
#include "sgftexture.h"
#include "sgfsound.h"
#include "button.h"
#include "tileresource.h"
#include <assert.h>
#include <math.h>
#include <tchar.h>

#pragma comment(lib, "dwrite")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "windowscodecs")


#define CITY_2048_WIDTH 500
#define CITY_2048_HEIGHT 750

#define RAND_RANAGE(a, b) (a + (rand()%(b-a+1)))
#define MIN_VALUE_IS_ONE(x) ((x <= 0) ? 1 : (x))

const int DEFAULT_TITLE_FONT_WIDTH = 80;
const int DEFAULT_SMAILL_TITLE_FONT_WIDTH = 50;
TCHAR g_game_tile[] = TEXT("日本建造");



City2048::City2048(HINSTANCE h)
	: SGF::GameApp(h), _titleBitmap(NULL), _effectBitmap(NULL), _currState(GS_INIT)
	, _titleAnimeIndex(0.0f), _waitRun(false), _cubeBitmap(NULL), _endBitmap(NULL)
	, _titleAnimeDuring(0.0f), _housePostionY(-tileHouse.size.height), _buttonIndex(0)
	, _buttonDrap(false), _startButtonDuring(0.0f), _isIncrease(true), _titleFormat(NULL)
	, _smallTitleFormat(NULL), _mainScoreFormat(NULL), _bestScoreFormat(NULL), _writeBrush(NULL)
	, _cubeFormat(NULL), _emptyBuildNum(VIEW_BUILD_SIZE*VIEW_BUILD_SIZE), _bestScore(0), _promptBitmap(NULL)
	, _currMoveNum(0), _isWin(false), _isDebug(false), _totalScore(0), _currScore(0), _hideLable(1.0f)
	, _statDuringTime(0.0f), _statEffectIndex(0), _statTotalTime(0.0f), _gameover(false)
{
	_win = { CITY_2048_WIDTH, CITY_2048_HEIGHT };
	srand((int)GetTickCount());

	for (int i = 0; i < BUTTON_MAX_COUNT; i++)
	{
		_buttonScale[i] = 1.0f;
	}
}

City2048::~City2048()
{

}

void City2048::gameUninitiate()
{
	destoryBitmaps();
	destoryFonts();

	_sound->closeBackgroundMusic();
}


void City2048::initCubesData()
{
	_emptyBuildNum = VIEW_BUILD_SIZE * VIEW_BUILD_SIZE;
	_currMoveNum = 0;

	for (int i = 0; i < VIEW_BUILD_SIZE; i++)
	{
		for (int j = 0; j < VIEW_BUILD_SIZE; j++)
		{
			_cubes[i][j].clear();
			_cubes[i][j].setPos(i, j);
			_cubes[i][j].setBitmap(_cubeBitmap);
		}
	}

	randomFillBuild();
	randomFillBuild();
	predictNextState();
}

void City2048::initGameData()
{
	_totalScore = 0;
	_currScore = 0;
	_isWin = false;
	_weatherBackground->setSeason(GS_SPRING);
	_weatherBackground->setTime(GT_FORENOON);

	initCubesData();
}


void City2048::initGameOverData()
{
	if (_totalScore > _bestScore)
		_bestScore = _totalScore;

	_hideLable = 1.0f;
	_statDuringTime = 0.0f;
	_statTotalTime = 0.0f;
	_statEffectIndex = 0;
	_gameover = false;
}


void City2048::initMenuData()
{
	_titleAnimeDuring = 0.0f;
	_titleAnimeIndex = 0;
	_housePostionY = -tileHouse.size.height;
	_buttonIndex = 0;
	_buttonDrap = false;
	_isIncrease = true;
	_startButtonDuring = 0.0f;
	_waitRun = false;

	for (int i = 0; i < BUTTON_MAX_COUNT; i++)
	{
		_buttonScale[i] = 1.0f;
	}

	_weatherBackground->setSeason(GS_SPRING);
	_weatherBackground->setTime(GT_FORENOON);
}


int City2048::gameInitiate()
{
	_baseWindow->setGameTitle(g_game_tile);
	_baseWindow->setWindowSize({ CITY_2048_WIDTH , CITY_2048_HEIGHT });
	_d2dRender->showFPS(false);

	//初始化字体资源
	if (createFonts() != 0) {
		return -1;
	}

	//初始化图片资源
	if (createBitmaps() != 0) {
		return -2;
	}

	//创建背景
	createBackground();

	//初始化建筑物
	initCubesData();

	//初始化按钮资源
	createButtons();

	//初始化事件监听
	initMouseListener();
	initKeyboardListener();

	//播放BGM
	_sound->playBackgroundMusic(TEXT("./sounds/BGM_InGame.wav"));

	return 0;
}


void City2048::gameUpdate(float dt)
{
	if (_currState == GS_INIT)
	{
		initMenuData();
		_currState = GS_MENU;
	}
	else if (_currState == GS_MENU)
	{
		_titleAnimeDuring += dt;
		if (_titleAnimeDuring > ANIME_FRAME_TIME)
		{
			_titleAnimeDuring -= ANIME_FRAME_TIME;

			if (_titleAnimeIndex < (sizeof(titleAnimas) / sizeof(TextureRect)) - 1)
			{
				_titleAnimeIndex++;
			}
			else
			{
				_titleAnimeIndex = (sizeof(titleAnimas) / sizeof(TextureRect)) - 1;

				//播放动画完毕，开始下落房子
				_housePostionY += 100;
				if (_housePostionY >= (_win.cy - tileHouse.size.height)*0.5)
				{
					_housePostionY = (_win.cy - tileHouse.size.height)*0.5;

					//房子下落完毕，开始显示按钮
					if (_buttonIndex < BUTTON_MAX_COUNT)
					{
						_buttonDrap = true;

						_buttonScale[_buttonIndex] -= 0.08f;
						if (_buttonScale[_buttonIndex] < BUTTON_MIN_SCALE) {
							_buttonScale[_buttonIndex] = BUTTON_MIN_SCALE;

							_buttonIndex++;
						}
					}
					//按钮显示完毕，显示开始按钮和标题
					else if (_buttonDrap)
					{
						_startButtonDuring += (_isIncrease ? dt : -dt);

						if (_startButtonDuring > (START_BUTTON_MIN_TIME + START_BUTTON_MAX_TIME) || _startButtonDuring < 0.0f) {
							_isIncrease = !_isIncrease;
						}


						//显示下落樱花
						_weatherBackground->drapPetals(dt);
						

						_waitRun = true;
					}
				}

				return;
			}
		}
	}
	else if (_currState == GS_RUN)
	{
		//更新背景
		_weatherBackground->tick(dt);


		//是否结束游戏
		for (int i = 0; i < VIEW_BUILD_SIZE; i++)
		{
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				if (_cubes[i][j].isWin())
				{
					_isWin = true;
				}

				_cubes[i][j].update();
			}
		}


		if (_currScore < _totalScore) {
			_currScore += MIN_VALUE_IS_ONE((_totalScore - _currScore) / 5);
			if (_currScore > _totalScore)
				_currScore = _totalScore;
		}

		if (_bestScore < _totalScore) {
			_bestScore += MIN_VALUE_IS_ONE((_totalScore - _bestScore) / 5);
			if (_bestScore > _totalScore)
				_bestScore = _totalScore;
		}


		if (_isWin)
		{
			_sound->playEffect(TEXT("./sounds/SE_Kansei.wav"));
			initGameOverData();
			_currState = GS_OVER;
		}
	}
	else if (_currState == GS_OVER)
	{
		//更新建筑物动画（放大）
		for (int i = 0; i < VIEW_BUILD_SIZE; i++)
		{
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				_cubes[i][j].update();
			}
		}


		_hideLable -= 0.1f;
		if (_hideLable < 0.0f) {
			_hideLable = 0.0f;
		}


		if (!_gameover)
		{
			_statDuringTime += dt;
			_statTotalTime += dt;
			if (_statDuringTime > 100.0f) {
				_statDuringTime -= 100.0f;
				_statEffectIndex = (_statEffectIndex + 1) % (ARRAYSIZE(starEffect));
			}

			if (_statTotalTime >= 1000.0f) {
				_gameover = true;
			}
		}
	}
}


void City2048::gameRender(float lag)
{
	if (_currState == GS_MENU)
	{
		//绘制背景
		_weatherBackground->draw(_d2dRender);


		ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
		renderTarget->DrawText(g_game_tile, _tcsclen(g_game_tile), _titleFormat,
			D2D1::RectF((_win.cx - DEFAULT_TITLE_FONT_WIDTH*4)*0.5f, 70, _win.cx, _win.cy), _writeBrush);


		//绘制动画
		_d2dRender->drawBitmap((_win.cx - TILE_ANIME_SIZE)*0.5f, (_win.cy - TILE_ANIME_SIZE)*0.5f,
			_titleBitmap, titleAnimas[_titleAnimeIndex].pos.x, titleAnimas[_titleAnimeIndex].pos.y,
			titleAnimas[_titleAnimeIndex].size.width, titleAnimas[_titleAnimeIndex].size.height);

		//绘制小房子
		_d2dRender->drawBitmap((_win.cx - tileHouse.size.width)*0.5f, _housePostionY,
			_titleBitmap, tileHouse.pos.x, tileHouse.pos.y, tileHouse.size.width, tileHouse.size.height);


		//绘制button
		if (_buttonDrap)
		{
			for (int i = 0; i <= _buttonIndex; i++)
			{
				_d2dRender->drawBitmap((_win.cx - TILE_BUTTON_SIZE * 4)*0.5f + iconButtons[i].size.width * (1.0f - _buttonScale[i]) * 0.5f + TILE_BUTTON_SIZE * i,
					_win.cy - TILE_BUTTON_SIZE - 40, iconButtons[i].size.width * _buttonScale[i], iconButtons[i].size.height * _buttonScale[i],
					_titleBitmap, iconButtons[i].pos.x, iconButtons[i].pos.y, iconButtons[i].size.width, iconButtons[i].size.height);
			}
		}

		
		//开始按钮
		if (_startButtonDuring >= START_BUTTON_MIN_TIME && _startButtonDuring <= START_BUTTON_MAX_TIME)
		{
			_d2dRender->drawBitmap((_win.cx - startButton.size.width)*0.5f, _win.cy - TILE_BUTTON_SIZE - 140, 
				_titleBitmap, startButton.pos.x, startButton.pos.y, startButton.size.width, startButton.size.height);
		}


		//叶子特效
		_weatherBackground->drawEffects(_d2dRender);
	}
	else if (_currState == GS_RUN)
	{
		//绘制背景
		_weatherBackground->draw(_d2dRender);
		
#if _DEBUG

		if (_isDebug)
		{
			//绘制空的个数（debug）
			ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
			_cubeFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
			TCHAR emptyText[128] = { 0 };
			size_t seconds = _weatherBackground->getSeconds();
			size_t h = (seconds / (60 * 60)) % 24;
			size_t m = ((seconds - h * 60 * 60) / 60) % 60;
			size_t s = (seconds - h * 60 * 60 - m * 60) % 60;

			_stprintf(emptyText, TEXT("时间：%.02u:%.02u:%.02u"), h, m, s);
			renderTarget->DrawText(emptyText, _tcsclen(emptyText), _cubeFormat,
				D2D1::RectF(0, _win.cy - 60, _win.cx-10, _win.cy), _writeBrush);


			_stprintf(emptyText, TEXT("天数：%.02u"), _weatherBackground->getDays());
			renderTarget->DrawText(emptyText, _tcsclen(emptyText), _cubeFormat,
				D2D1::RectF(0, _win.cy - 40, _win.cx - 10, _win.cy), _writeBrush);
		}

#endif



		//绘制阴影
		float drawWidth = shadowRect.size.width * CUBE_SCALE_SIZE;
		float drawHeight = shadowRect.size.height * CUBE_SCALE_SIZE;
		_d2dRender->drawBitmap((_win.cx - drawWidth)*0.5f, (_win.cy - drawHeight)*0.5f+60, drawWidth, drawHeight,
			_cubeBitmap, shadowRect.pos.x, shadowRect.pos.y, shadowRect.size.width, shadowRect.size.height);
		
#if _DEBUG

		if (_isDebug)
		{
			//绘制空的个数（debug）
			ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
			_cubeFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			TCHAR emptyText[128] = { 0 };
			_stprintf(emptyText, TEXT("空建筑数：%d 本次移动数：%d "), _emptyBuildNum, _currMoveNum);
			renderTarget->DrawText(emptyText, _tcsclen(emptyText), _cubeFormat,
				D2D1::RectF(0, _win.cy - 60, _win.cx, _win.cy), _writeBrush);
			memset(emptyText, 0, sizeof(emptyText));
			_stprintf(emptyText, TEXT("下次移动（上/下/左/右）：%d %d %d %d"),
				_predictNext[0].moveNum, _predictNext[1].moveNum, _predictNext[2].moveNum, _predictNext[3].moveNum);
			renderTarget->DrawText(emptyText, _tcsclen(emptyText), _cubeFormat,
				D2D1::RectF(0, _win.cy - 40, _win.cx, _win.cy), _writeBrush);
			memset(emptyText, 0, sizeof(emptyText));
			_stprintf(emptyText, TEXT("下次合并（上/下/左/右）：%d %d %d %d"),
				_predictNext[0].mergeNum, _predictNext[1].mergeNum, _predictNext[2].mergeNum, _predictNext[3].mergeNum);
			renderTarget->DrawText(emptyText, _tcsclen(emptyText), _cubeFormat,
				D2D1::RectF(0, _win.cy - 20, _win.cx, _win.cy), _writeBrush);
		}

#endif



		//绘制方块
		float cubesWidth = CUBE_SCALE_SIZE * cubeRects[0].size.width * VIEW_BUILD_SIZE;
		int count = 1, layer = 0;
		bool flag = true;
		while (count > 0)
		{
			for (int index = 0; index < count; index++)
			{
				int gapX = 0, gapY = BUILD_GAP_SIZE;
				if (2 * index + 1 > count)
					gapX = BUILD_GAP_SIZE;
				else if (2 * index + 1 < count)
					gapX = -BUILD_GAP_SIZE;
				else
					gapX = 0;

				if (flag) 
				{
					_cubes[index][VIEW_BUILD_SIZE - count + index].draw(_d2dRender, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
#if _DEBUG
					if (_isDebug)
					{
						_cubes[index][VIEW_BUILD_SIZE - count + index].drawDebugInfo(_d2dRender, _cubeFormat, _writeBrush, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
					}
#endif
				}	
				else 
				{
					_cubes[VIEW_BUILD_SIZE - count + index][index].draw(_d2dRender, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
#if _DEBUG
					if (_isDebug)
					{
						_cubes[VIEW_BUILD_SIZE - count + index][index].drawDebugInfo(_d2dRender, _cubeFormat, _writeBrush, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
					}
#endif
				}
					
			}

			if (count == VIEW_BUILD_SIZE)
				flag = !flag;

			flag ? count++ : count--;

			layer++;
		}

		ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();

		//显示小标题
		renderTarget->DrawText(g_game_tile, _tcsclen(g_game_tile), _smallTitleFormat,
			D2D1::RectF((_win.cx - DEFAULT_SMAILL_TITLE_FONT_WIDTH * 4)*0.5f, _win.cy - 130, _win.cx, _win.cy), _writeBrush);

		_weatherBackground->drawEffects(_d2dRender);


		//绘制按钮
		_backButton->draw(_d2dRender);

		//绘制分数面板
		_d2dRender->drawBitmap(-10, 50, mainScore.size.width * 0.75f,
			mainScore.size.height * 0.75f, _cubeBitmap, mainScore.pos.x,
			mainScore.pos.y, mainScore.size.width, mainScore.size.height);

		_d2dRender->drawBitmap(-10, 150, bestScore.size.width * 0.75f,
			bestScore.size.height * 0.75f, _cubeBitmap, bestScore.pos.x,
			bestScore.pos.y, bestScore.size.width, bestScore.size.height);


		
		//显示分数
		TCHAR scoreText[128] = { 0 };

		_stprintf(scoreText, TEXT("%d"), _currScore);
		renderTarget->DrawText(scoreText, _tcsclen(scoreText), _mainScoreFormat,
			D2D1::RectF(10, 75, mainScore.size.width * 0.75f - 20, mainScore.size.height * 0.75f + 80),
			_blackBrush);


		_stprintf(scoreText, TEXT("%d"), _bestScore);
		renderTarget->DrawText(scoreText, _tcsclen(scoreText), _bestScoreFormat,
			D2D1::RectF(10, 165, bestScore.size.width * 0.75f - 20, bestScore.size.height * 0.75f + 165),
			_blackBrush);
	}
	else if (_currState == GS_OVER)
	{
		//绘制背景
		_weatherBackground->draw(_d2dRender);


		//绘制方块
		float drawWidth = shadowRect.size.width * CUBE_SCALE_SIZE;
		float drawHeight = shadowRect.size.height * CUBE_SCALE_SIZE;
		float cubesWidth = CUBE_SCALE_SIZE * cubeRects[0].size.width * VIEW_BUILD_SIZE;
		int count = 1, layer = 0;
		bool flag = true;
		while (count > 0)
		{
			for (int index = 0; index < count; index++)
			{
				int gapX = 0, gapY = BUILD_GAP_SIZE;
				if (2 * index + 1 > count)
					gapX = BUILD_GAP_SIZE;
				else if (2 * index + 1 < count)
					gapX = -BUILD_GAP_SIZE;
				else
					gapX = 0;

				if (flag)
				{
					_cubes[index][VIEW_BUILD_SIZE - count + index].draw(_d2dRender, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
#if _DEBUG
					if (_isDebug)
					{
						_cubes[index][VIEW_BUILD_SIZE - count + index].drawDebugInfo(_d2dRender, _cubeFormat, _writeBrush, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
					}
#endif
				}
				else
				{
					_cubes[VIEW_BUILD_SIZE - count + index][index].draw(_d2dRender, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
#if _DEBUG
					if (_isDebug)
					{
						_cubes[VIEW_BUILD_SIZE - count + index][index].drawDebugInfo(_d2dRender, _cubeFormat, _writeBrush, (_win.cx - cubesWidth)*0.5f + (abs(2 * index - count + 1))*gapX, (_win.cy - drawHeight)*0.5f - 40 + layer * gapY);
					}
#endif
				}

			}

			if (count == VIEW_BUILD_SIZE)
				flag = !flag;

			flag ? count++ : count--;

			layer++;
		}


		if (_hideLable > 0.0f)
		{
			//绘制按钮
			_backButton->draw(_d2dRender, _hideLable);

			//绘制分数面板
			_d2dRender->drawBitmap(-10, 50, mainScore.size.width * 0.75f,
				mainScore.size.height * 0.75f, _cubeBitmap, mainScore.pos.x,
				mainScore.pos.y, mainScore.size.width, mainScore.size.height, _hideLable);

			_d2dRender->drawBitmap(-10, 150, bestScore.size.width * 0.75f,
				bestScore.size.height * 0.75f, _cubeBitmap, bestScore.pos.x,
				bestScore.pos.y, bestScore.size.width, bestScore.size.height, _hideLable);


			ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
			//显示分数
			TCHAR scoreText[128] = { 0 };
			auto bcolor = _blackBrush->GetColor();
			bcolor.a = _hideLable;
			_blackBrush->SetColor(bcolor);

			_stprintf(scoreText, TEXT("%d"), _currScore);
			renderTarget->DrawText(scoreText, _tcsclen(scoreText), _mainScoreFormat,
				D2D1::RectF(10, 75, mainScore.size.width * 0.75f - 20, mainScore.size.height * 0.75f + 80),
				_blackBrush);

			_stprintf(scoreText, TEXT("%d"), _bestScore);
			renderTarget->DrawText(scoreText, _tcsclen(scoreText), _bestScoreFormat,
				D2D1::RectF(10, 165, bestScore.size.width * 0.75f - 20, bestScore.size.height * 0.75f + 165),
				_blackBrush);

			bcolor.a = 1.0f;
			_blackBrush->SetColor(bcolor);


			//显示小标题
			auto wcolor = _writeBrush->GetColor();
			wcolor.a = _hideLable;
			_writeBrush->SetColor(wcolor);
			renderTarget->DrawText(g_game_tile, _tcsclen(g_game_tile), _smallTitleFormat,
				D2D1::RectF((_win.cx - DEFAULT_SMAILL_TITLE_FONT_WIDTH * 4)*0.5f, _win.cy - 130, _win.cx, _win.cy), _writeBrush);
			wcolor.a = 1.0f;
			_writeBrush->SetColor(wcolor);
		}
		else
		{
			//闪烁一秒星星
			if (!_gameover)
			{
				_d2dRender->drawBitmap(0, 0, _promptBitmap, starEffect[_statEffectIndex].pos.x,
					starEffect[_statEffectIndex].pos.y, starEffect[_statEffectIndex].size.width, starEffect[_statEffectIndex].size.height);
			}
			else
			{
				//绘制完成文字
				_d2dRender->drawBitmap((_win.cx - finishText.size.width * 1.5f)*0.5f, 130,
					finishText.size.width * 1.5f, finishText.size.height * 1.5f,
					_cubeBitmap, finishText.pos.x, finishText.pos.y, finishText.size.width,
					finishText.size.height);

				//绘制按钮
				_restartButton->draw(_d2dRender);
			}
		}

	}
	else if (_currState == GS_END)
	{
		_d2dRender->drawBitmap(0, 0, _win.cx, _win.cy, _endBitmap, endBg.pos.x,
			endBg.pos.y, endBg.size.width, endBg.size.height);

		float startX = (_win.cx - endScore.size.width * 0.75f) * 0.5f;
		_d2dRender->drawBitmap(startX, 150, endScore.size.width * 0.75f,
			endScore.size.height * 0.75f, _endBitmap, endScore.pos.x, endScore.pos.y, endScore.size.width, endScore.size.height);
	
		TCHAR scoreText[128] = { 0 };
		_stprintf(scoreText, TEXT("%d"), _currScore);
		ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
		auto old = _mainScoreFormat->GetTextAlignment();
		_mainScoreFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		renderTarget->DrawText(scoreText, _tcsclen(scoreText), _mainScoreFormat,
			D2D1::RectF(180, 160, 400, 180+50), _blackBrush);
		_mainScoreFormat->SetTextAlignment(old);

		//绘制按钮
		_endHomeButton->draw(_d2dRender);
		_endRetryButton->draw(_d2dRender);
		_endShareButton->draw(_d2dRender);


		//新纪录
		if (_totalScore == _bestScore && _bestScore != 0)
		{
			const D2D1::Matrix3x2F rot = D2D1::Matrix3x2F::Rotation(-45.0f,
				D2D1::Point2F(endRecord.size.width * 0.5f, endRecord.size.height * 0.5f));
			const D2D1::Matrix3x2F trans = D2D1::Matrix3x2F::Translation(0, 140);

			renderTarget->SetTransform(rot * trans);
		
			_d2dRender->drawBitmap(0, 0, _endBitmap, endRecord.pos.x,
				endRecord.pos.y, endRecord.size.width, endRecord.size.height);
			renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		}

	}
}



void City2048::initMouseListener()
{
	auto mouseListener = SGF::EventListenerMouse::create();
	mouseListener->onMouseClick = [this](SGF::EventMouse* event) {
		if (_currState == GS_MENU && _waitRun)
		{
			_sound->playEffect(TEXT("./sounds/SE_button.wav"));
			initGameData();
			_currState = GS_RUN;
		}
		else if (_currState == GS_RUN)
		{
			if (_backButton->press())
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				_backButton->press(false);
				_currState = GS_INIT;
			}
		}
		else if (_currState == GS_OVER && _gameover)
		{
			if (_restartButton->press())
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				_restartButton->press(false);
				_currState = GS_END;
			}
		}
		else if (_currState == GS_END)
		{
			if (_endHomeButton->press())
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				_endHomeButton->press(false);
				_currState = GS_INIT;
			}
			else if (_endRetryButton->press())
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				initGameData();
				_endRetryButton->press(false);
				_currState = GS_RUN;
			}
			else if (_endShareButton->press())
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				_endShareButton->press(false);
			}
		}
	};

	mouseListener->onMouseMove = [this](SGF::EventMouse* event) {
		if (_currState == GS_RUN)
		{
			if (_backButton->hitTest(event->getCursorX(), event->getCursorY()))
			{
				_backButton->press(true);
			}
			else
			{
				_backButton->press(false);
			}
		}
		else if (_currState == GS_OVER)
		{
			if (_restartButton->hitTest(event->getCursorX(), event->getCursorY()))
			{
				_restartButton->press(true);
			}
			else
			{
				_restartButton->press(false);
			}
		}
		else if (_currState == GS_END)
		{
			if (_endHomeButton->hitTest(event->getCursorX(), event->getCursorY()))
			{
				_endHomeButton->press(true);
			}
			else
			{
				_endHomeButton->press(false);
			}


			if (_endRetryButton->hitTest(event->getCursorX(), event->getCursorY()))
			{
				_endRetryButton->press(true);
			}
			else
			{
				_endRetryButton->press(false);
			}


			if (_endShareButton->hitTest(event->getCursorX(), event->getCursorY()))
			{
				_endShareButton->press(true);
			}
			else
			{
				_endShareButton->press(false);
			}
		}
	};

	_eventDispatcher->addEventListener(mouseListener);
}

void City2048::initKeyboardListener()
{

	auto keyboardListener = SGF::EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = [this](SGF::EventKeyboard::KeyCode code, SGF::Event*) {

		if (_currState == GS_MENU)
		{
			if (code == SGF::EventKeyboard::KeyCode::KEY_SPACE && _waitRun)
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				initGameData();
				_currState = GS_RUN;
			}
		}
		else if (_currState == GS_OVER)
		{
			if (code == SGF::EventKeyboard::KeyCode::KEY_SPACE && _gameover)
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				_currState = GS_END;
			}
		}
		else if (_currState == GS_END)
		{
			if (code == SGF::EventKeyboard::KeyCode::KEY_SPACE)
			{
				_sound->playEffect(TEXT("./sounds/SE_button.wav"));
				initGameData();
				_currState = GS_RUN;
			}
		}
		else if (_currState == GS_RUN)
		{
			//操作
			MERGE_RESULT mr = { 0 };
			if (code == SGF::EventKeyboard::KeyCode::KEY_LEFT)
			{
				mr = std::move(mergeLeft());
			}
			else if (code == SGF::EventKeyboard::KeyCode::KEY_RIGHT)
			{
				mr = std::move(mergeRight());
			}
			else if (code == SGF::EventKeyboard::KeyCode::KEY_UP)
			{
				mr = std::move(mergeUp());
			}
			else if (code == SGF::EventKeyboard::KeyCode::KEY_DOWN)
			{
				mr = std::move(mergeDown());
			}
			else if (code == SGF::EventKeyboard::KeyCode::KEY_F1)
			{
				_isDebug = !_isDebug;
				if(_d2dRender)
					_d2dRender->showFPS(_isDebug);
			}


			//移动
			if (mr.moveNum) {
				_sound->playEffect(TEXT("./sounds/SE_Move1.wav"));
				randomFillBuild();
			}


			//合并
			if (mr.mergeNum) {
				_sound->playEffect(TEXT("./sounds/SE_combine.wav"));
			}


			_emptyBuildNum += mr.mergeNum;
			_currMoveNum = mr.moveNum;


			//预测
			if (!predictNextState()) {
				_sound->playEffect(TEXT("./sounds/SE_Kansei.wav"));
				_isWin = false;
				initGameOverData();
				_currState = GS_OVER;
			}
		}


	};
	_eventDispatcher->addEventListener(keyboardListener);
}


void City2048::createBackground()
{
	_weatherBackground = std::make_shared<Weather>(_cubeBitmap, _effectBitmap, _win);
	_weatherBackground->setSeason(GS_SPRING);
	_weatherBackground->setTime(GT_FORENOON);
}


void City2048::createButtons()
{
	_backButton = std::make_shared<Button>(_cubeBitmap, backMenuButton);
	TextureRect renderRect = {
		{_win.cx - backMenuButton.size.width + 30, 45},
		{backMenuButton.size.width * 0.65f, backMenuButton.size.height * 0.65f}
	};
	_backButton->setPostion(renderRect);


	_restartButton = std::make_shared<Button>(_cubeBitmap, playGameButton);
	renderRect = {
		{_win.cx - playGameButton.size.width - 30.0f, _win.cy - playGameButton.size.height - 40},
		{playGameButton.size.width * 1.0f, playGameButton.size.height * 1.0f}
	};
	_restartButton->setPostion(renderRect);


	float btnStartX = (_win.cx - endScore.size.width * 0.75f) * 0.5f;
	float btnGap = (endScore.size.width * 0.75f - endRetry.size.width * 0.75f * 3) * 0.5f;

	_endHomeButton = std::make_shared<Button>(_endBitmap, endHome);
	renderRect = {
		{btnStartX, 350.0f}, {endHome.size.width * 0.75f, endHome.size.height * 0.75f }
	};
	_endHomeButton->setPostion(renderRect);

	_endRetryButton = std::make_shared<Button>(_endBitmap, endRetry);
	renderRect = {
		{btnStartX + btnGap + endRetry.size.width * 0.75f, 350.0f}, {endRetry.size.width * 0.75f, endRetry.size.height * 0.75f }
	};
	_endRetryButton->setPostion(renderRect);

	_endShareButton = std::make_shared<Button>(_endBitmap, endShare);
	renderRect = {
		{btnStartX + btnGap * 2 + endRetry.size.width * 0.75f * 2, 350.0f}, {endShare.size.width * 0.75f, endShare.size.height * 0.75f }
	};
	_endShareButton->setPostion(renderRect);
}


int City2048::createBitmaps()
{
	int ret = _d2dRender->loadBitmapFromFile(TEXT("./images/title.png"), 0, 0, &_titleBitmap);
	if (ret != 0) {
		return -1;
	}


	ret = _d2dRender->loadBitmapFromFile(TEXT("./images/effects.png"), 0, 0, &_effectBitmap);
	if (ret != 0) {
		destoryBitmaps();
		return -2;
	}


	ret = _d2dRender->loadBitmapFromFile(TEXT("./images/cubes.png"), 0, 0, &_cubeBitmap);
	if (ret != 0) {
		destoryBitmaps();
		return -3;
	}

	ret = _d2dRender->loadBitmapFromFile(TEXT("./images/prompts.png"), 0, 0, &_promptBitmap);
	if (ret != 0) {
		destoryBitmaps();
		return -4;
	}

	
	ret = _d2dRender->loadBitmapFromFile(TEXT("./images/end.png"), 0, 0, &_endBitmap);
	if (ret != 0) {
		destoryBitmaps();
		return -5;
	}

	return 0;
}


void City2048::destoryBitmaps()
{
	SGF::D2DSafeRelease(&_titleBitmap);
	SGF::D2DSafeRelease(&_effectBitmap);
	SGF::D2DSafeRelease(&_cubeBitmap);
	SGF::D2DSafeRelease(&_promptBitmap);
	SGF::D2DSafeRelease(&_endBitmap);
}


int City2048::createFonts()
{
	IDWriteFactory* dWriteFactory = _d2dRender->getWriteFactory();
	if (!dWriteFactory) {
		return -1;
	}


	ID2D1HwndRenderTarget* renderTarget = _d2dRender->getHwndRenderTarget();
	if (!renderTarget) {
		return -2;
	}


	HRESULT hr = dWriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_HEAVY,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_TITLE_FONT_WIDTH, L"", &_titleFormat);
	if (FAILED(hr)) {
		return -3;
	}


	hr = dWriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_SMAILL_TITLE_FONT_WIDTH, L"", &_smallTitleFormat);
	if (FAILED(hr)) {
		destoryFonts();
		return -4;
	}

	hr = dWriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_BLACK,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 50, L"", &_mainScoreFormat);
	if (FAILED(hr)) {
		destoryFonts();
		return -5;
	}
	_mainScoreFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);


	hr = dWriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 40, L"", &_bestScoreFormat);
	if (FAILED(hr)) {
		destoryFonts();
		return -6;
	}
	_bestScoreFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);

	hr = dWriteFactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_HEAVY,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULT_DEBUG_FONT_WIDTH, L"", &_cubeFormat);
	if (FAILED(hr)) {
		destoryFonts();
		return -7;
	}


	hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_writeBrush);
	if (FAILED(hr)) {
		destoryFonts();
		return -8;
	}

	hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0x5C747A), &_blackBrush);
	if (FAILED(hr)) {
		destoryFonts();
		return -9;
	}
		
	return 0;
}


void City2048::destoryFonts()
{
	SGF::D2DSafeRelease(&_writeBrush);
	SGF::D2DSafeRelease(&_blackBrush);
	SGF::D2DSafeRelease(&_cubeFormat);
	SGF::D2DSafeRelease(&_titleFormat);
	SGF::D2DSafeRelease(&_smallTitleFormat);
	SGF::D2DSafeRelease(&_mainScoreFormat);
	SGF::D2DSafeRelease(&_bestScoreFormat);
}


/*
 * 数组 [0 -> VIEW_BUILD_SIZE]，合并方向 -> 0
 */
MERGE_RESULT City2048::mergeCubes(const std::array<Cube*, VIEW_BUILD_SIZE>& cubes, size_t& score)
{
	int index = 0;
	MERGE_RESULT mr = { 0 };
	score = 0;

	//查找第一个需要合并的建筑物，从索引0方向开始，合并方向为：VIEW_BUILD_SIZE -> 0
	while (index < VIEW_BUILD_SIZE)
	{
		//为空，查看下一个
		if (cubes[index]->empty()) {
			index++;
			continue;
		}


		//找到建筑物，向0方向合并
		int mergeBuildIndex = index;
		
		//越过空
		int pos = mergeBuildIndex - 1;
		while (pos >= 0 && cubes[pos]->empty())
			pos--;

		size_t s = 0;
		if (pos < 0 || !cubes[pos]->merge(*(cubes[mergeBuildIndex]), s))
		{
			cubes[pos + 1]->swap(*(cubes[mergeBuildIndex]));
			if(pos + 1 != mergeBuildIndex)
				mr.moveNum++;
		}
		else if (pos >= 0)
		{
			score += s;
			mr.mergeNum++;
			mr.moveNum++;
		}

		index++;
	}

	return mr;
}

bool City2048::predictNextState()
{
	_predictNext[0] = mergeUp(true);
	_predictNext[1] = mergeDown(true);
	_predictNext[2] = mergeLeft(true);
	_predictNext[3] = mergeRight(true);


	for (int i = 0; i < 4; i++)
	{
		if (_predictNext[i].mergeNum != 0
			|| _predictNext[i].moveNum != 0)
			return true;
	}

	return false;
}

void City2048::randomFillBuild()
{
	if (_emptyBuildNum <= 0)
		return;

	CUBE_TYPE ct = rand() % 2 ? CT_4 : CT_2;
	int pos = rand() % _emptyBuildNum;
	int r = 0, c = 0, index = 0;
	while (true)
	{
		r = index / VIEW_BUILD_SIZE;
		c = index % VIEW_BUILD_SIZE;

		if (_cubes[r][c].empty()) {
			pos--;
		}
			
		if (pos < 0) {
			break;
		}

		index = (index + 1) % (VIEW_BUILD_SIZE * VIEW_BUILD_SIZE);
	}


	assert(r >= 0 && r < VIEW_BUILD_SIZE);
	assert(c >= 0 && c < VIEW_BUILD_SIZE);
	assert(_cubes[r][c].empty());

	_cubes[r][c].type(ct);

	_emptyBuildNum--;
}


MERGE_RESULT City2048::mergeLeft(bool predict)
{
	MERGE_RESULT result = { 0 };
	MERGE_RESULT mr = { 0 };
	size_t s = 0;
	for (int i = 0; i < VIEW_BUILD_SIZE; i++)
	{
		if (predict)
		{
			std::array<Cube, VIEW_BUILD_SIZE> mocks;
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				mocks[j] = _cubes[i][j];
				group[j] = &mocks[j];
			}

			mr = std::move(mergeCubes(group, s));
		}
		else
		{
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				group[j] = &_cubes[i][j];
			}

			mr = std::move(mergeCubes(group, s));
			_totalScore += s;
		}

		result.mergeNum += mr.mergeNum;
		result.moveNum += mr.moveNum;
	}

	return result;
}


MERGE_RESULT City2048::mergeRight(bool predict)
{
	MERGE_RESULT result = { 0 };
	MERGE_RESULT mr = { 0 };
	size_t s = 0;
	for (int i = 0; i < VIEW_BUILD_SIZE; i++)
	{
		if (predict)
		{
			std::array<Cube, VIEW_BUILD_SIZE> mocks;
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				mocks[j] = _cubes[i][VIEW_BUILD_SIZE-j-1];
				group[j] = &mocks[j];
			}

			mr = std::move(mergeCubes(group, s));
		}
		else
		{
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				group[j] = &_cubes[i][VIEW_BUILD_SIZE - j - 1];
			}

			mr = std::move(mergeCubes(group, s));
			_totalScore += s;
		}

		result.mergeNum += mr.mergeNum;
		result.moveNum += mr.moveNum;
	}

	return result;
}


MERGE_RESULT City2048::mergeDown(bool predict)
{
	MERGE_RESULT result = { 0 };
	MERGE_RESULT mr = { 0 };
	size_t s = 0;
	for (int i = 0; i < VIEW_BUILD_SIZE; i++)
	{
		if (predict)
		{
			std::array<Cube, VIEW_BUILD_SIZE> mocks;
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				mocks[j] = _cubes[VIEW_BUILD_SIZE - j - 1][i];
				group[j] = &mocks[j];
			}

			mr = std::move(mergeCubes(group, s));
		}
		else
		{
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				group[j] = &_cubes[VIEW_BUILD_SIZE - j - 1][i];
			}

			mr = std::move(mergeCubes(group, s));
			_totalScore += s;
		}

		result.mergeNum += mr.mergeNum;
		result.moveNum += mr.moveNum;
	}

	return result;
}


MERGE_RESULT City2048::mergeUp(bool predict)
{
	MERGE_RESULT result = { 0 };
	MERGE_RESULT mr = { 0 };
	size_t s = 0;
	for (int i = 0; i < VIEW_BUILD_SIZE; i++)
	{
		if (predict)
		{
			std::array<Cube, VIEW_BUILD_SIZE> mocks;
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				mocks[j] = _cubes[j][i];
				group[j] = &mocks[j];
			}

			mr = std::move(mergeCubes(group, s));
		}
		else
		{
			std::array<Cube*, VIEW_BUILD_SIZE> group;
			for (int j = 0; j < VIEW_BUILD_SIZE; j++)
			{
				group[j] = &_cubes[j][i];
			}

			mr = std::move(mergeCubes(group, s));
			_totalScore += s;
		}

		result.mergeNum += mr.mergeNum;
		result.moveNum += mr.moveNum;
	}

	return result;
}