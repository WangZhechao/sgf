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
	//λ��
	POINT _pos;
	//״̬
	BlockState _state;
	//����
	BlockType _type;


public:
	Block();
	Block(const Block& block);
	Block(BlockType type, BlockState state);
	Block& operator = (const Block& block);


public:
	//��ת
	bool rotate();

	//����
	bool dropDown();

	//����
	bool leftMove();

	//����
	bool rightMove();
};