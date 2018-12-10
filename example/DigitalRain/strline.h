#pragma once

#include <Windows.h>
#include <list>

class StrLine
{
	friend class DigitalRain;

private:
	int _length;					//长度
	int _vy;						//速度
	int _waiTime;				//等待时间
	POINT _beginPos;			//起始位置
	std::list<char> _charList;	//数字列表

public:
	StrLine(int x, int y);

public:
	CHAR genChar();				//生成字符
	void genLine(int y = 0);    //生成字符串
	void setPosX(int x);		//设置水平位置
};