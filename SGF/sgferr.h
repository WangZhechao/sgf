#pragma once

namespace SGF
{
	typedef int ERR_TYPE;

	const ERR_TYPE ERR_NONE							= 0;				//无错误
	const ERR_TYPE ERR_REGISTER_CLASS_FAIL			= -1;				//注册窗口类失败
	const ERR_TYPE ERR_ADJUST_WIN_FAIL				= -2;				//调整窗口失败
	const ERR_TYPE ERR_CREATE_WIN_FAIL				= -3;				//创建窗口失败
	const ERR_TYPE ERR_CREATE_D2D_DEVICE_FAIL		= -4;				//创建设备资源失败
	const ERR_TYPE ERR_INITIATE_D2D_DEVICE_FAIL		= -5;				//创建设备资源失败
	const ERR_TYPE ERR_LOAD_DLL_FAIL = -6;
}

