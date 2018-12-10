#pragma once

#include <Windows.h>
#include <list>

class StrLine
{
	friend class DigitalRain;

private:
	int _length;					//����
	int _vy;						//�ٶ�
	int _waiTime;				//�ȴ�ʱ��
	POINT _beginPos;			//��ʼλ��
	std::list<char> _charList;	//�����б�

public:
	StrLine(int x, int y);

public:
	CHAR genChar();				//�����ַ�
	void genLine(int y = 0);    //�����ַ���
	void setPosX(int x);		//����ˮƽλ��
};