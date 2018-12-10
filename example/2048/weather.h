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
	GT_MORN = 0,	//���� ǳ��	5 ~ 8, ������Ч
	GT_FORENOON,	//���� ǳ��	8 ~ 11, ��Ҷ��Ч, ��
	GT_NOON,		//���� ǳ��	11 ~ 13
	GT_AFTERNOON,	//���� ��ɫ	13 ~ 17	��Ҷ��Ч����
	GT_DUSK,		//�ƻ� ��ɫ	17 ~ 20 ��
	GT_NIGHT		//���� ����	20 ~ 5	������Ч
};


enum GAME_SEASON
{
	GS_SPRING = 0,	//����	ӣ��
	GS_SUMMER,		//����
	GS_AUTUMN,		//����  ��Ҷ
	GS_WINTER		//����	ѩ
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

