#include "sgfdl.h"

namespace SGF
{
	DLLModule::DLLModule()
		: _module(NULL)
	{
	}

	DLLModule::DLLModule(const TCHAR* filename)
		: _module(NULL)
	{

	}

	DLLModule::~DLLModule()
	{
		close();
	}

	ERR_TYPE DLLModule::open(const TCHAR * filename)
	{
		_module = LoadLibraryEx(filename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (_module == NULL)
			return ERR_LOAD_DLL_FAIL;

		return ERR_NONE;
	}


	void DLLModule::close()
	{
		if (_module) 
		{
			FreeLibrary(_module);
			_module = NULL;
		}
	}


	void * DLLModule::getSym(const char * name)
	{
		void *ptr = (void*)GetProcAddress(_module, name);
		
		return ptr;
	}
}

