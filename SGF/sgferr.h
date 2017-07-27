#pragma once

namespace SGF
{
	typedef int ERR_TYPE;

	const ERR_TYPE ERR_NONE							= 0;				//�޴���
	const ERR_TYPE ERR_REGISTER_CLASS_FAIL			= -1;				//ע�ᴰ����ʧ��
	const ERR_TYPE ERR_ADJUST_WIN_FAIL				= -2;				//��������ʧ��
	const ERR_TYPE ERR_CREATE_WIN_FAIL				= -3;				//��������ʧ��
	const ERR_TYPE ERR_CREATE_D2D_DEVICE_FAIL		= -4;				//�����豸��Դʧ��
	const ERR_TYPE ERR_INITIATE_D2D_DEVICE_FAIL		= -5;				//�����豸��Դʧ��
	const ERR_TYPE ERR_LOAD_DLL_FAIL = -6;
}

