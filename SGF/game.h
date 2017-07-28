#pragma once

#include <Windows.h>

const int DEFAULT_FPS = 30;


namespace SGF
{
	class Render;
}


typedef int(*getfps)();
typedef const TCHAR*(*getname)();

typedef void(*init)(SGF::Render* render);
typedef void(*input)(float dt);
typedef bool(*update)(float dt);
typedef void(*render)(SGF::Render* render, float dt);
typedef void(*uninit)(SGF::Render* render);


//��Ϸ�ӿ�
struct GameInterface
{
	getname getname;	//��ȡ����
	getfps getfps;		//��ȡ����֡
	

	init init;			//��ʼ��
	input input;		//�������
	update update;		//�����߼�
	render render;		//��Ⱦͼ��
	uninit uninit;		//����ʼ��
};


typedef GameInterface*(*GetInterfaceFun)();