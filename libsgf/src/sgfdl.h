#pragma once

#include <Windows.h>
#include "sgferr.h"

namespace SGF
{
	class DLLModule
	{
	private:
		HMODULE _module;

	public:
		DLLModule();
		DLLModule(const TCHAR* filename);
		virtual ~DLLModule();

		DLLModule(const DLLModule&) = delete;
		DLLModule& operator=(const DLLModule&) = delete;

	public:
		ERR_TYPE open(const TCHAR* filename);
		void close();
		void* getSym(const char* name);
	};
}