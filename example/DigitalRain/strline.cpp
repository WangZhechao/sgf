#include "strline.h"

#define MAX_LINE_LENGTH	20
#define MIN_LINE_LENGTH 10

#define MAX_SPEED_Y 15
#define MIN_SPEED_Y 5

#define MAX_WAIT_TIME 20

StrLine::StrLine(int x, int y)
	: _beginPos({ x, y })
{
	genLine(y);
}


CHAR StrLine::genChar()
{
	return CHAR(rand() % ('~' - '!') + '!');
}


void StrLine::genLine(int y)
{
	_length = rand() % (MAX_LINE_LENGTH - MIN_LINE_LENGTH) + MAX_LINE_LENGTH;
	_vy = rand() % (MAX_SPEED_Y - MIN_SPEED_Y) + MIN_SPEED_Y;
	if (y == 0)
		_waiTime = rand() % MAX_WAIT_TIME;
	else
		_waiTime = 0;

	_beginPos.y = y;

	_charList.clear();
	for (int i = 0; i < _length; i++)
	{
		_charList.push_back(genChar());
	}
}


void StrLine::setPosX(int x)
{
	_beginPos.x = x;
}
