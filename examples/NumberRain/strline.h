#pragma once

#include <Windows.h>
#include <list>

class StrLine
{
	friend class NumRain;

private:
	int _length;					//长度
	int _vy;						//速度
	int _wait_time;				//等待时间
	POINT _begin_pos;			//起始位置
	std::list<char> _num_list;	//数字列表

public:
	StrLine(int x);

public:
	CHAR genChar();				//生成字符
	void genLine();				//生成字符串
	void setPosX(int x);		//设置水平位置
};