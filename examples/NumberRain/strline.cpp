#include "stdafx.h"
#include "strline.h"

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
	_length = rand() % 5 + 10;
	_vy = rand() % 10 + 5;
	_wait_time = rand() % 20;
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
