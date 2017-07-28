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


//游戏接口
struct GameInterface
{
	getname getname;	//获取名称
	getfps getfps;		//获取更新帧
	

	init init;			//初始化
	input input;		//检测输入
	update update;		//更新逻辑
	render render;		//渲染图像
	uninit uninit;		//反初始化
};


typedef GameInterface*(*GetInterfaceFun)();