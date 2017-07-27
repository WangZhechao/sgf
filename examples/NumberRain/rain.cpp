#include "stdafx.h"
#include "rain.h"
#include "strline.h"
#include "sgfrender.h"


NumRain::NumRain(const SIZE& win, const SIZE& font)
	: _textformat(NULL)
{
	_win = win;
	_font = font;

	srand((int)GetTickCount()); //初始化随机数发生器
}

NumRain::~NumRain()
{
	
}


void NumRain::genLines()
{
	int count = _win.cx / _font.cx;

	_num_lines.clear();
	for (int i = 0; i < count; i++)
	{
		_num_lines.push_back(StrLine(i * _font.cx));
	}
}


bool NumRain::createFont(SGF::Render* render)
{
	if (render)
	{
		_textformat = render->CreateTextFormat();
	}

	if (_textformat)
		return true;

	return false;
}


void NumRain::destoryFont(SGF::Render* render)
{
	if (render)
	{
		render->DestoryTextFormat(_textformat);
	}
}


void NumRain::update()
{
	std::vector<StrLine>::iterator itr;

	for (itr = _num_lines.begin(); itr != _num_lines.end(); ++itr)
	{
		if (itr->_wait_time > 0)
		{
			itr->_wait_time--;
		}
		else if (itr->_wait_time <= 0)
		{
			itr->_num_list.push_front(itr->genChar());
			itr->_num_list.pop_back();

			itr->_begin_pos.y += itr->_vy;
		}


		//越界
		if (itr->_begin_pos.y - (itr->_length * _font.cy) > _win.cy)
		{
			itr->genLine();
		}
	}
}


void NumRain::draw(SGF::Render* render)
{
	if (render == NULL)
		return;

	TCHAR ctmp[2] = {0};

	std::vector<StrLine>::iterator vitr;
	for (vitr = _num_lines.begin(); vitr != _num_lines.end(); ++vitr)
	{
		if (vitr->_wait_time > 0)
			continue;


		std::list<char>::iterator litr;
		int count = 1;
		for (litr = vitr->_num_list.begin(); litr != vitr->_num_list.end(); ++litr)
		{
			POINT point = {vitr->_begin_pos.x, vitr->_begin_pos.y - _font.cy * count};
			
			if(count == 1)
				render->SetTextColor(_textformat, RGB(255, 255, 255));
			else
				render->SetTextColor(_textformat, RGB(0, 255 - 255 / (vitr->_length)*count, 0));


			if ((point.x >= 0 && point.x <= _win.cx - _font.cx)
				&& (point.y >= 0 && point.y <= _win.cy - _font.cy))
			{
				ctmp[0] = *litr;
				render->DrawText(point.x, point.y, ctmp, _textformat);
			}

			count++;
		}
	}
}