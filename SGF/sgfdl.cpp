#include "sgfdl.h"
#include "sgferr.h"

namespace SGF
{
	DLLModule::DLLModule()
		: m_hModule(NULL)
	{
	}

	DLLModule::DLLModule(const TCHAR* filename)
		: m_hModule(NULL)
	{

	}

	DLLModule::~DLLModule()
	{
		Close();
	}

	int DLLModule::Open(const TCHAR * filename)
	{
		m_hModule = LoadLibraryEx(filename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (m_hModule == NULL)
			return ERR_LOAD_DLL_FAIL;

		return 0;
	}


	void DLLModule::Close()
	{
		if (m_hModule) 
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}


	void * DLLModule::GetSym(const char * name)
	{
		void *ptr = (void*)GetProcAddress(m_hModule, name);
		
		return ptr;
	}
}

