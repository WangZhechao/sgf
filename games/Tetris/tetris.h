#pragma once

#include "block.h"

enum class TetrisState
{
	TS_INIT = 0,		//��ʼ��
	TS_RUN,				//����
	TS_PAUSE,			//��ͣ
	TS_DEAD				//����
};


const int TETRIS_CONTAINER_WIDTH = 10;
const int TETRIS_CONTAINER_HEIGHT = 24;
const int TETRIS_CONTAINER_HIDE_HEIGHT = BLOCK_HEIGHT;

const int TETRIS_MAX_SCORE = 999999; //��߷�
const int TETRIS_MAX_LEVEL = 10;	 //��ߵȼ�
const int TETRIS_MAX_LINE = 999999;	 //�����


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
	//��ʼ��
	void init();

	//����
	void update();

	//��Ⱦ
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