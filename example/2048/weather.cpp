#include "weather.h"
#include "tileresource.h"

Weather::Weather(ID2D1Bitmap* cubesBitmap, ID2D1Bitmap* effectsBitmap, const SIZE& win)
	: _cubesBitmap(cubesBitmap), _effectsBitmap(effectsBitmap), _win(win), _effectMornIndex(-1)
	, _currSeason(GS_SPRING), _totalSeconds(0), _totalMornDuring(0.0f), _effectNightIndex(0)
	, _totalNightDuring(0.0f), _transition(1.0f), _totalDays(0)
{
	setTime(GT_FORENOON);
}


Weather::~Weather()
{
}


void Weather::draw(const std::shared_ptr<SGF::D2DRender>& d2dRender)
{
	//绘制背景
	d2dRender->drawBitmap(0, 0, _cubesBitmap, timeRect[_currTime].pos.x,
		timeRect[_currTime].pos.y, timeRect[_currTime].size.width, timeRect[_currTime].size.height, _transition);

	//绘制上一个背景（用于过渡）
	if (_transition < 1.0f) {
		GAME_TIME prevTime = (GAME_TIME)(((int)_currTime + (int)GT_NIGHT) % (int)(GT_NIGHT + 1));
		d2dRender->drawBitmap(0, 0, _cubesBitmap, timeRect[prevTime].pos.x,
			timeRect[prevTime].pos.y, timeRect[prevTime].size.width, timeRect[prevTime].size.height, 1.0f - _transition);
	}


	//云
	for (auto itr = _coluds.begin(); itr != _coluds.end(); ++itr)
	{
		int index = itr->index + ((_currTime == GT_AFTERNOON || _currTime == GT_DUSK) ? 2 : 0);
		d2dRender->drawBitmap(itr->x, itr->y, dayCloud[index].size.width * 0.6f,
			dayCloud[index].size.height * 0.6f, _cubesBitmap, dayCloud[index].pos.x,
			dayCloud[index].pos.y, dayCloud[index].size.width, dayCloud[index].size.height, 0.85f);
	}

}


void Weather::drawEffects(const std::shared_ptr<SGF::D2DRender>& d2dRender)
{
	if (_currTime == GT_MORN && _effectMornIndex >= 0 && _effectMornIndex < 5)
	{
		size_t index = _effectMornIndex % ARRAYSIZE(sunshineRect);
		d2dRender->drawBitmap(0, 0, _win.cx, _win.cy, _effectsBitmap, sunshineRect[index].pos.x,
			sunshineRect[index].pos.y, sunshineRect[index].size.width, sunshineRect[index].size.height);
	}


	if (_currTime == GT_NIGHT && _effectNightIndex >= 0)
	{
		d2dRender->drawBitmap(5, 20, _win.cx, 0, _cubesBitmap, starRect[_effectNightIndex].pos.x, starRect[_effectNightIndex].pos.y,
			starRect[_effectNightIndex].size.width, starRect[_effectNightIndex].size.height);

		d2dRender->drawBitmap(5, 100, 0, 0, _cubesBitmap, starRect[_effectNightIndex].pos.x, starRect[_effectNightIndex].pos.y,
			starRect[_effectNightIndex].size.width, starRect[_effectNightIndex].size.height);
	}



	for (auto itr = _draps.begin(); itr != _draps.end(); ++itr)
	{
		d2dRender->getHwndRenderTarget()->SetTransform(D2D1::Matrix3x2F::Rotation(itr->rotation,
			D2D1::Point2F(itr->x + drapRects[itr->index].size.width * 0.5f, itr->y + drapRects[itr->index].size.height * 0.5f)));
		d2dRender->drawBitmap(itr->x, itr->y, drapRects[itr->index].size.width * 0.75f, drapRects[itr->index].size.height * 0.75f, _effectsBitmap, drapRects[itr->index].pos.x,
			drapRects[itr->index].pos.y, drapRects[itr->index].size.width, drapRects[itr->index].size.height);
		d2dRender->getHwndRenderTarget()->SetTransform(D2D1::Matrix3x2F::Identity());
	}

}


void Weather::drapPetals(float dt)
{
	int drapIndex = 0;
	size_t drapCount = 10;
	int drapRand = 30;
	float speedx = 0.2f + rand() % 5 * 0.1f;
	float speedy = 1.5f + rand() % 15 * 0.1f;


	if (_draps.size() < drapCount && rand() % drapRand == 0) {
		_draps.emplace_back(std::move(SpriteEffect({
			10.0f + (rand() % (_win.cy - 20)),
			-20.0f,
			speedx,
			speedy,
			2.0f,
			1.0f + rand() % 5 * 1.0f,
			drapIndex
		})));
	}


	auto itr = _draps.begin();
	while (itr != _draps.end())
	{
		itr->x += itr->speedx;
		itr->y += itr->speedy;
		itr->rotation += itr->speedr;

		if (itr->x > _win.cx || itr->x < -20
			|| itr->y > _win.cy)
		{
			itr = _draps.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}


void Weather::tick(float dt)
{
	_totalSeconds += 30;	//每次滴答，代表30s

	if (_totalSeconds > 24 * 60 * 60)
		_totalSeconds -= 24 * 60 * 60;

	if (_totalDays > 4 * 7)
		_totalDays -= 4 * 7;

	if (_transition < 1.0f) {
		_transition += 0.1f;
	}


	//推算时间
	if (_totalSeconds > 5 * 60 * 60 && _totalSeconds <= 8 * 60 * 60) 
	{
		if (_currTime == GT_NIGHT) {
			_totalDays++;
			_currSeason = GAME_SEASON(_totalDays / 7);
			_transition = 0.0f;
		}
		_currTime = GT_MORN;
	}
	else if (_totalSeconds > 8 * 60 * 60 && _totalSeconds <= 11 * 60 * 60)
	{
		_currTime = GT_FORENOON;
	}
	else if (_totalSeconds > 11 * 60 * 60 && _totalSeconds <= 13 * 60 * 60)
	{
		_currTime = GT_NOON;
	}
	else if (_totalSeconds > 13 * 60 * 60 && _totalSeconds <= 17 * 60 * 60)
	{
		if (_currTime == GT_NOON) {
			_transition = 0.0f;
		}
		_currTime = GT_AFTERNOON;
	}
	else if (_totalSeconds > 17 * 60 * 60 && _totalSeconds <= 20 * 60 * 60)
	{
		_currTime = GT_DUSK;
	}
	else
	{
		if (_currTime == GT_DUSK) {
			_transition = 0.0f;
		}
		_currTime = GT_NIGHT;
	}


	//云
	if (_currTime == GT_FORENOON || _currTime == GT_AFTERNOON || _currTime == GT_DUSK)
	{
		//添加云
		if (_coluds.size() < 3 && rand() % 150 == 0) {
			_coluds.emplace_back(std::move(SpriteEffect({
				-200.0f,
				100.0f + (rand() % 2 ? 20.0f : 0.0f),
				rand() % 2 + 0.5f,
				0.0f,
				0.0f,
				0.0f,
				rand() % 2
				})));
		}
	}
	else if (_currTime == GT_NIGHT)
	{
		_coluds.clear();
	}


	//更新云
	auto itr = _coluds.begin();
	while (itr != _coluds.end())
	{
		itr->x += itr->speedx;

		if (itr->x > _win.cx)
		{
			itr = _coluds.erase(itr);
		}
		else
		{
			++itr;
		}
	}


	//落叶
	if ((_currTime == GT_FORENOON || _currTime == GT_AFTERNOON) && ((_totalDays % (1+rand()%3)) == 0))
	{
		int drapIndex = 0;
		size_t drapCount = 0;
		int drapRand = 0;
		float speedx = 0.0f;
		float speedy = 0.0f;

		if (_currSeason == GS_SPRING)
		{
			drapIndex = 0;
			drapCount = 8;
			drapRand = 30;
			speedx = 0.2f + rand() % 5 * 0.1f;
			speedy = 1.0f + rand() % 5 * 0.1f;
		}
		else if (_currSeason == GS_AUTUMN)
		{
			drapIndex = 1 + rand() % 2;
			drapCount = 15;
			drapRand = 15;
			speedx = -0.7f + rand() % 5 * 0.1f;
			speedy = 1.2f + rand() % 12 * 0.1f;
		}
		else if (_currSeason == GS_WINTER)
		{
			drapIndex = 3 + rand() % 2;
			drapCount = 50;
			drapRand = 8;
			speedx = -0.2f + rand() % 5 * 0.1f;
			speedy = 1.8f + rand() % 12 * 0.1f;
		}

		
		
		if (_draps.size() < drapCount && rand() % drapRand == 0) {
			_draps.emplace_back(std::move(SpriteEffect({					
				10.0f + (rand() % (_win.cy - 20)),
				-20.0f,
				speedx,
				speedy,
				2.0f,
				1.0f + rand() % 5 * 1.0f,
				drapIndex
			})));
		}
	}

	itr = _draps.begin();
	while (itr != _draps.end())
	{
		itr->x += itr->speedx;
		itr->y += itr->speedy;
		itr->rotation += itr->speedr;

		if (itr->x > _win.cx || itr->x < -20
			|| itr->y > _win.cy)
		{
			itr = _draps.erase(itr);
		}
		else
		{
			++itr;
		}
	}


	//早上
	if (_currTime == GT_MORN)
	{
		_totalMornDuring += dt;

		if (_effectMornIndex < 5)
		{
			if (_totalMornDuring > 600.0f)
			{
				_totalMornDuring -= 600.0f;
				_effectMornIndex++;
				if (_effectMornIndex > 5) {
					_totalMornDuring = 0.0f;
				}
			}
		}
		else
		{
			if (_totalMornDuring > 1000.0f) {
				_totalMornDuring = 0.0f;
				_effectMornIndex = -1;
			}
		}
	}
	else
	{
		_effectMornIndex = -1;
		_totalMornDuring = 0.0f;
	}


	//晚上
	if (_currTime == GT_NIGHT)
	{
		_totalNightDuring += dt;
		if (_totalNightDuring > 300.0f)
		{
			_totalNightDuring -= 300.0f;
			_effectNightIndex = (_effectNightIndex + 1) % ARRAYSIZE(starRect);
		}
	}
	else
	{
		_effectNightIndex = -1;
		_totalNightDuring = 0.0f;
	}
}


void Weather::setTime(GAME_TIME t)
{ 
	_currTime = t; 
	_transition = 1.0f;
	_totalDays = 0;
	_coluds.clear();
	_draps.clear();
	
	switch (_currTime)
	{
	case GT_MORN: _totalSeconds = 5 * 60 * 60;  break;
	case GT_FORENOON: _totalSeconds = 8 * 60 * 60;  break;
	case GT_NOON: _totalSeconds = 11 * 60 * 60;  break;
	case GT_AFTERNOON: _totalSeconds = 13 * 60 * 60;  break;
	case GT_DUSK: _totalSeconds = 17 * 60 * 60;  break;
	default:
		_totalSeconds = 20 * 60 * 60;
	}

	_effectMornIndex = 0;
	_totalMornDuring = 0.0f;

	_effectNightIndex = 0;
	_totalNightDuring = 0.0f;
}

void Weather::setSeason(GAME_SEASON s)
{ 
	_currSeason = s; 
}