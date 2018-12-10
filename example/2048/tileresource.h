#pragma once

#include "sgftexture.h"


struct TextureRect
{
	D2D1_POINT_2F pos;
	D2D1_SIZE_F size;
};


//���⶯��
extern TextureRect titleAnimas[17];

//��ť
extern TextureRect iconButtons[];

//����
extern TextureRect tileHouse;


//��ʼ��ť
extern TextureRect startButton;

//����
extern TextureRect sunshineRect[3];
//���� 
extern TextureRect starRect[3];
//����
extern TextureRect petalRect;

extern TextureRect drapRects[];
//��
extern TextureRect dayCloud[2];
extern TextureRect duskCloud[2];

//ʱ�䱳��
extern TextureRect timeRect[];


//��Ӱ
extern TextureRect shadowRect;


//����
extern TextureRect cubeRects[];

//����
extern TextureRect mainScore;
extern TextureRect bestScore;


//���˵���ť
extern TextureRect backMenuButton;
extern TextureRect playGameButton;

//�������
extern TextureRect finishText;

extern TextureRect starEffect[3];


//����
extern TextureRect endBg;
//����
extern TextureRect endScore;
//��ҳ
extern TextureRect endHome;
//����
extern TextureRect endRetry;
//����
extern TextureRect endShare;
//��¼
extern TextureRect endRecord;


extern const int BUILD_GAP_SIZE;
extern const int TILE_BUTTON_SIZE;
extern const int TILE_ANIME_SIZE;
extern const float CUBE_SCALE_SIZE;
extern const float BUTTON_MIN_SCALE;
extern const float ANIME_FRAME_TIME;
extern const float START_BUTTON_MIN_TIME;
extern const float START_BUTTON_MAX_TIME;


