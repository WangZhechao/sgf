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
	Block(const POINT& pos, BlockType type, BlockState state);
	Block& operator = (const Block& block);


private:
	//�ϲ�
	void merge(Tetris *tetris);

	//��ײ���
	bool hitTest(Tetris *tetris);


public:
	//��ת
	bool rotate(Tetris*);

	//����
	bool drop(Tetris*);

	//����
	bool downMove(Tetris*);

	//����
	bool leftMove(Tetris*);

	//����
	bool rightMove(Tetris*);
};