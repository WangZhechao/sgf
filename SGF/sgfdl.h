#pragma once

#include <Windows.h>

namespace SGF
{
	class DLLModule
	{
	private:
		HMODULE m_hModule;

	public:
		DLLModule();
		DLLModule(const TCHAR* filename);
		~DLLModule();

	public:
		int Open(const TCHAR* filename);
		void Close();
		void* GetSym(const char* name);
	};
}