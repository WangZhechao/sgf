#pragma once

#include <Windows.h>
class SGF::Render;


typedef int(*getfps)();
typedef void(*init)();
typedef void(*input)(float dt);
typedef bool(*update)(float dt);
typedef void(*render)(SGF::Render* render, float dt);
typedef void(*uninit)();


//游戏接口
struct GameInterface
{
	getfps getfps;		//获取信息
	init init;			//初始化
	input input;		//检测输入
	update update;		//更新逻辑
	render render;		//渲染图像
	uninit uninit;		//反初始化
};


//获取游戏接口
GameInterface* getGameInterface();