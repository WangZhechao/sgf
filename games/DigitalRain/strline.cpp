#include "strline.h"

#define MAX_LINE_LENGTH	20
#define MIN_LINE_LENGTH 10

#define MAX_SPEED_Y 15
#define MIN_SPEED_Y 5

#define MAX_WAIT_TIME 20

StrLine::StrLine(int x)
	: _begin_pos({x, 0})
{
	genLine();
}


CHAR StrLine::genChar()
{
	return CHAR(rand() % ('~' - '!') + '!');
}


void StrLine::genLine()
{
	_length = rand() % (MAX_LINE_LENGTH - MIN_LINE_LENGTH) + MAX_LINE_LENGTH;
	_vy = rand() % (MAX_SPEED_Y- MIN_SPEED_Y) + MIN_SPEED_Y;
	_wait_time = rand() % MAX_WAIT_TIME;
	_begin_pos.y = 0;

	_num_list.clear();
	for (int i = 0; i < _length; i++)
	{
		_num_list.push_back(genChar());
	}
}


void StrLine::setPosX(int x)
{
	_begin_pos.x = x;
}
