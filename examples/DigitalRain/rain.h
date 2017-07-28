#pragma once

#include <vector>
#include "strline.h"

namespace SGF
{
	class Render;
	
	typedef void TextFormat;
}



class DigitalRain
{
private:
	SIZE _font;
	SIZE _win;
	SGF::TextFormat* _textformat;

	std::vector<StrLine> _num_lines;

public:
	DigitalRain(const SIZE& win, const SIZE& font);
	~DigitalRain();

public:
	void genLines();
	bool createFont(SGF::Render* render);
	void destoryFont(SGF::Render* render);
	void update();
	void draw(SGF::Render* render);
};