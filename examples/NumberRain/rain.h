#pragma once

#include <vector>
#include "strline.h"

namespace SGF
{
	class Render;
	
	typedef void TextFormat;
}



class NumRain
{
private:
	SIZE _font;
	SIZE _win;
	SGF::TextFormat* _textformat;

	std::vector<StrLine> _num_lines;

public:
	NumRain(const SIZE& win, const SIZE& font);
	~NumRain();

public:
	void genLines();
	bool createFont(SGF::Render* render);
	void destoryFont(SGF::Render* render);
	void update();
	void draw(SGF::Render* render);
};