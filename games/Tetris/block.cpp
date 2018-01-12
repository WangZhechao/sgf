#include "block.h"
#include "tetris.h"


/*
 * 2018��1��11��20:55:42 ����д���룬��˯��������ֵܣ�װ�ƾ�����һ������
 */



/*
 * ���ָ����ת��ʽ��һ��unsigned short���ʹ���һ�����飬4 * 4 �����ʾ��
 * Ϊʲôʹ�ø÷�������ά���鲻��ô����װ�����ŵ���......
 *
 * [ 0 0 0 0 ]
 * [ 0 0 0 0 ]
 * [ 0 0 0 0 ]
 * [ 0 0 0 0 ]
 */
static unsigned short g_blackList[BlockType::BT_NUM][BlockState::BS_NUM] = {
	{0x04C8, 0x00C6, 0x04C8, 0x00C6}, //S
	{0x08C4, 0x006C, 0x08C4, 0x006C}, //Z
	{0x0C44, 0x00E8, 0x088C, 0x002E}, //J
	{0x0C88, 0x00E2, 0x044C, 0x008E}, //L
	{0x8888, 0x000F, 0x8888, 0x000F}, //I
	{0x004E, 0x04C4, 0x00E4, 0x08C8}, //T
	{0x00CC, 0x00CC, 0x00CC, 0x00CC}  //O
};


Block::Block()
	: _state(BS_T)
	, _type(BT_S)
{
	_pos.x = 0;
	_pos.y = 0;
}


Block::Block(const Block & block)
{
	*this = block;
}


Block::Block(BlockType type, BlockState state)
	: _type(type), _state(state)
{
	_pos.x = 0;
	_pos.y = 0;
}

Block::Block(const POINT & pos, BlockType type, BlockState state)
	: _pos(pos), _type(type), _state(state)
{
}


Block & Block::operator=(const Block & block)
{
	this->_pos = block._pos;
	this->_state = block._state;
	this->_type = block._type;

	return *this;
}


void Block::merge(Tetris * tetris)
{
	unsigned short block = g_blackList[_type][_state];

	//�ϲ�
	int index = 0;
	while (index < BLOCK_HEIGHT)
	{
		tetris->_blockContainer[_pos.y + index] &= (((block >> (index * BLOCK_WIDTH)) & 0x000F)
			<< (TETRIS_CONTAINER_WIDTH - BLOCK_WIDTH - _pos.x));

		index++;
	}
}

bool Block::hitTest(Tetris * tetris)
{
	unsigned short block = g_blackList[_type][_state];

	for (int i = 0; i < BLOCK_HEIGHT; i++)
	{
		//����ײ
		if (tetris->_blockContainer[_pos.y]
			& (((block >> i * BLOCK_WIDTH) & 0x000F)
				<< (TETRIS_CONTAINER_WIDTH - BLOCK_WIDTH - _pos.x)))
		{
			return true;
		}
	}

	return false;
}


bool Block::rotate(Tetris *tetris)
{
	BlockState nextState = static_cast<BlockState>((_state + 1) % BS_NUM);
	Block next(_pos, _type, nextState);

	//����ײ
	if (next.hitTest(tetris))
	{
		return false;
	}

	_state = next._state;

	return true;
}


bool Block::drop(Tetris *tetris)
{
	while (downMove(tetris)) {}

	return true;
}


bool Block::downMove(Tetris *tetris)
{
	//Խ��
	int y = _pos.y + 1;
	if (y >= TETRIS_CONTAINER_HEIGHT - BLOCK_HEIGHT) {
		merge(tetris);
		return false;
	}

	Block next({_pos.x, y}, _type, _state);

	//����ײ
	if (next.hitTest(tetris))
	{
		//�ϲ�
		merge(tetris);

		return false;
	}

	return true;
}


bool Block::leftMove(Tetris *tetris)
{
	//Խ��
	int x = _pos.x - 1;
	if (x <= 0) {
		merge(tetris);
		return false;
	}

	Block next({ x, _pos.y}, _type, _state);

	//����ײ
	if (next.hitTest(tetris))
	{
		//�ϲ�
		merge(tetris);

		return false;
	}

	return true;
}


bool Block::rightMove(Tetris *tetris)
{
	//Խ��
	int x = _pos.x + 1;
	if (x >= TETRIS_CONTAINER_WIDTH - BLOCK_WIDTH) {
		merge(tetris);
		return false;
	}

	Block next({ x, _pos.y }, _type, _state);

	//����ײ
	if (next.hitTest(tetris))
	{
		//�ϲ�
		merge(tetris);

		return false;
	}

	return true;
}
