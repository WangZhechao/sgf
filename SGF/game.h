#pragma once

#include <Windows.h>
class SGF::Render;


typedef int(*getfps)();
typedef void(*init)();
typedef void(*input)(float dt);
typedef bool(*update)(float dt);
typedef void(*render)(SGF::Render* render, float dt);
typedef void(*uninit)();


//��Ϸ�ӿ�
struct GameInterface
{
	getfps getfps;		//��ȡ��Ϣ
	init init;			//��ʼ��
	input input;		//�������
	update update;		//�����߼�
	render render;		//��Ⱦͼ��
	uninit uninit;		//����ʼ��
};


//��ȡ��Ϸ�ӿ�
GameInterface* getGameInterface();