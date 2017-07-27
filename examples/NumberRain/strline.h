#pragma once

#include <Windows.h>
#include <list>

class StrLine
{
	friend class NumRain;

private:
	int _length;					//����
	int _vy;						//�ٶ�
	int _wait_time;				//�ȴ�ʱ��
	POINT _begin_pos;			//��ʼλ��
	std::list<char> _num_list;	//�����б�

public:
	StrLine(int x);

public:
	CHAR genChar();
	void genLine();
	void setPosX(int x);		//����ˮƽλ��
};