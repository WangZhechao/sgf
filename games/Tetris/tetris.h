#pragma once

#include "block.h"

enum class TetrisState
{
	TS_INIT = 0,		//初始化
	TS_RUN,				//运行
	TS_PAUSE,			//暂停
	TS_DEAD				//死亡
};


const int TETRIS_CONTAINER_WIDTH = 10;
const int TETRIS_CONTAINER_HEIGHT = 24;
const int TETRIS_CONTAINER_HIDE_HEIGHT = BLOCK_HEIGHT;

const int TETRIS_MAX_SCORE = 999999; //最高分
const int TETRIS_MAX_LEVEL = 10;	 //最高等级
const int TETRIS_MAX_LINE = 999999;	 //最大行


class Tetris
{
	friend class Block;

private:
	unsigned long _blockContainer[TETRIS_CONTAINER_HEIGHT];
	Block _blocks[2];
	TetrisState _state;
	int _level;
	unsigned long _score;
	unsigned long _lines;
	bool _muted;


public:
	//初始化
	void init();

	//更新
	void update();

	//渲染
	void render();


public:
	void setSound(bool mute)
	{
		_muted = mute;
	}

	void setState(TetrisState state)
	{
		_state = state;
	}


	unsigned long getCurrentScore()
	{
		return _score;
	}

	unsigned long getCurrentLevel()
	{
		return _level;
	}

	unsigned long getCurrentLines()
	{
		return _lines;
	}
};