#pragma once

#include "sgftexture.h"


struct TextureRect
{
	D2D1_POINT_2F pos;
	D2D1_SIZE_F size;
};


//标题动画
extern TextureRect titleAnimas[17];

//按钮
extern TextureRect iconButtons[];

//房子
extern TextureRect tileHouse;


//开始按钮
extern TextureRect startButton;

//阳光
extern TextureRect sunshineRect[3];
//星星 
extern TextureRect starRect[3];
//花瓣
extern TextureRect petalRect;

extern TextureRect drapRects[];
//云
extern TextureRect dayCloud[2];
extern TextureRect duskCloud[2];

//时间背景
extern TextureRect timeRect[];


//阴影
extern TextureRect shadowRect;


//方块
extern TextureRect cubeRects[];

//分数
extern TextureRect mainScore;
extern TextureRect bestScore;


//主菜单按钮
extern TextureRect backMenuButton;
extern TextureRect playGameButton;

//完成文字
extern TextureRect finishText;

extern TextureRect starEffect[3];


//背景
extern TextureRect endBg;
//分数
extern TextureRect endScore;
//主页
extern TextureRect endHome;
//重玩
extern TextureRect endRetry;
//分享
extern TextureRect endShare;
//记录
extern TextureRect endRecord;


extern const int BUILD_GAP_SIZE;
extern const int TILE_BUTTON_SIZE;
extern const int TILE_ANIME_SIZE;
extern const float CUBE_SCALE_SIZE;
extern const float BUTTON_MIN_SCALE;
extern const float ANIME_FRAME_TIME;
extern const float START_BUTTON_MIN_TIME;
extern const float START_BUTTON_MAX_TIME;


