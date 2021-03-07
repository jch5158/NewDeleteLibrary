#include "stdafx.h"
#include <process.h>

DWORD WINAPI WorkerThread(void* pParam)
{
	int* ptr = new int;

	delete ptr;

	return 1;
}

INT main()
{
	_wsetlocale(LC_ALL, L"");

	CCrashDump::GetInstance();

	CSystemLog::GetInstance()->SetLogDirectory(L"NewLibrary");

	CSystemLog::GetInstance()->SetLogLevel(CSystemLog::eLogLevel::LogLevelDebug);

	int* p = new int;

	*p = 10;

	wprintf_s(L"%d\n", *p);

	//CNewLibrary::PrintNewInfo();

	std::unordered_map<void*, int> oMap;
	
	void* ptr = malloc(4);

	oMap.insert(std::make_pair(ptr, 10));

	oMap.erase(ptr);

	delete p;

	wprintf_s(L"asdf\n");


	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)WorkerThread, NULL, NULL, NULL);

	WaitForSingleObject(handle, INFINITE);

	CNewLibrary::PrintNewInfo();

	//return 1;

	TerminateProcess(GetCurrentProcess(), 1);
}
