#pragma once

#include <Windows.h>
#include <bitset>

enum class BlockType
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


enum class BlockState
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
	Block& operator = (const Block& block);


public:
	//旋转
	bool rotate();

	//下落
	bool dropDown();

	//左移
	bool leftMove();

	//右移
	bool rightMove();
};