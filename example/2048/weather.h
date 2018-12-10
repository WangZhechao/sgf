#pragma once

#include <d2d1helper.h>
#include <d2d1.h>
#include <list>
#include "sgfd2drender.h"
#include "sgftexture.h"


struct SpriteEffect
{
	float x;
	float y;
	float speedx;
	float speedy;
	float rotation;
	float speedr;
	int index;
};


enum GAME_TIME
{
	GT_MORN = 0,	//早上 浅蓝	5 ~ 8, 阳光特效
	GT_FORENOON,	//上午 浅蓝	8 ~ 11, 树叶特效, 云
	GT_NOON,		//中午 浅蓝	11 ~ 13
	GT_AFTERNOON,	//下午 橙色	13 ~ 17	树叶特效，云
	GT_DUSK,		//黄昏 橙色	17 ~ 20 云
	GT_NIGHT		//晚上 深蓝	20 ~ 5	星星特效
};


enum GAME_SEASON
{
	GS_SPRING = 0,	//春天	樱花
	GS_SUMMER,		//夏天
	GS_AUTUMN,		//秋天  落叶
	GS_WINTER		//冬天	雪
};


class Weather
{
public:
	Weather(ID2D1Bitmap* cubesBitmap, ID2D1Bitmap* effectsBitmap, const SIZE& win);
	virtual ~Weather();

public:
	void drapPetals(float dt);
	void tick(float dt);
	void draw(const std::shared_ptr<SGF::D2DRender>& d2dRender);
	void drawEffects(const std::shared_ptr<SGF::D2DRender>& d2dRender);
	void setTime(GAME_TIME t);
	void setSeason(GAME_SEASON s);
	GAME_TIME getTime() { return _currTime; }
	GAME_SEASON getSeason() { return _currSeason; }
	size_t getSeconds() { return _totalSeconds; }
	size_t getDays() { return _totalDays; }

private:
	GAME_SEASON _currSeason;
	GAME_TIME _currTime;

	float _totalMornDuring;
	int _effectMornIndex;

	float _totalNightDuring;
	int _effectNightIndex;

	size_t _totalSeconds;
	size_t _totalDays;
	SIZE _win;

	float _transition;

	std::list< SpriteEffect > _coluds;
	std::list< SpriteEffect > _draps;

	ID2D1Bitmap* _cubesBitmap;
	ID2D1Bitmap* _effectsBitmap;
};

