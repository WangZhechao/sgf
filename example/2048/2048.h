#pragma once

#include "sgfgameapp.h"
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>
#include <array>
#include "cube.h"
#include "weather.h"

const int BUTTON_MAX_COUNT = 4;
const int MAX_PETAL_COUNT = 6;


enum GAME_STATE
{
	GS_INIT = 0,
	GS_MENU,
	GS_RUN,
	GS_OVER,
	GS_END
};


struct MERGE_RESULT
{
	unsigned short moveNum;
	unsigned short mergeNum;
};

class Button;

class City2048 : public SGF::GameApp
{
public:
	City2048(HINSTANCE);
	~City2048();


public:
	virtual int gameInitiate();
	virtual void gameUpdate(float dt);
	virtual void gameRender(float lag);
	virtual void gameUninitiate();



private:
	//合并一组建筑物（一列或者一排）
	MERGE_RESULT mergeCubes(const std::array<Cube*, VIEW_BUILD_SIZE>& cubes, size_t& score);
	MERGE_RESULT mergeLeft(bool predict = false);
	MERGE_RESULT mergeRight(bool predict = false);
	MERGE_RESULT mergeUp(bool predict = false);
	MERGE_RESULT mergeDown(bool predict = false);

	//随机生成建筑物
	void randomFillBuild();

	//预测下一个状态，true 继续 false 结束
	bool predictNextState();

	//初始化状态
	void initCubesData();
	void initGameData();
	void initMenuData();
	void initGameOverData();


private:
	int _emptyBuildNum;
	int _currMoveNum;
	MERGE_RESULT _predictNext[4];
	Cube _cubes[VIEW_BUILD_SIZE][VIEW_BUILD_SIZE];
	size_t _totalScore;
	size_t _currScore;
	size_t _bestScore;
	bool _isWin;
	float _hideLable;
	float _statDuringTime;
	float _statTotalTime;
	int _statEffectIndex;
	bool _gameover;
	std::shared_ptr< Weather> _weatherBackground;

private:
	std::shared_ptr<Button> _backButton;
	std::shared_ptr<Button> _restartButton;

	std::shared_ptr<Button> _endHomeButton;
	std::shared_ptr<Button> _endRetryButton;
	std::shared_ptr<Button> _endShareButton;

private:
	int createFonts();
	void destoryFonts();

	int createBitmaps();
	void destoryBitmaps();

	void createButtons();
	void createBackground();

	void initMouseListener();
	void initKeyboardListener();

private:
	int _titleAnimeIndex;
	int _housePostionY;
	float _titleAnimeDuring;
	float _buttonScale[BUTTON_MAX_COUNT];
	int _buttonIndex;
	bool _buttonDrap;
	float _startButtonDuring;
	bool _isIncrease;
	bool _waitRun;
	bool _isDebug;
	IDWriteTextFormat* _cubeFormat;
	IDWriteTextFormat* _titleFormat;
	IDWriteTextFormat* _smallTitleFormat;
	IDWriteTextFormat* _mainScoreFormat;
	IDWriteTextFormat* _bestScoreFormat;
	
	ID2D1SolidColorBrush* _writeBrush;
	ID2D1SolidColorBrush* _blackBrush;

private:
	SIZE _win;
	GAME_STATE _currState;
	ID2D1Bitmap* _endBitmap;
	ID2D1Bitmap* _titleBitmap;
	ID2D1Bitmap* _effectBitmap;
	ID2D1Bitmap* _cubeBitmap;
	ID2D1Bitmap* _promptBitmap;
};