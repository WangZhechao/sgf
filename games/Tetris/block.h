#pragma once

#include <Windows.h>

class Tetris;

enum BlockType
{
	BT_S = 0,
	BT_Z,
	BT_J,
	BT_L,
	BT_I,
	BT_T,
	BT_O,
	BT_NUM
};


enum BlockState
{
	BS_T = 0,
	BS_R,
	BS_B,
	BS_L,
	BS_NUM
};

const int BLOCK_WIDTH = 4;
const int BLOCK_HEIGHT = 4;

class Block
{
private:
	//位置
	POINT _pos;
	//状态
	BlockState _state;
	//类型
	BlockType _type;


public:
	Block();
	Block(const Block& block);
	Block(BlockType type, BlockState state);
	Block(const POINT& pos, BlockType type, BlockState state);
	Block& operator = (const Block& block);


private:
	//合并
	void merge(Tetris *tetris);

	//碰撞检测
	bool hitTest(Tetris *tetris);


public:
	//旋转
	bool rotate(Tetris*);

	//下落
	bool drop(Tetris*);

	//下移
	bool downMove(Tetris*);

	//左移
	bool leftMove(Tetris*);

	//右移
	bool rightMove(Tetris*);
};