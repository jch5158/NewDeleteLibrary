#pragma once

#include <iostream>
#include <Windows.h>
#include <unordered_map>
#include "List/CListLib/CList.h"

#include "DumpLibrary/DumpLibrary/CCrashDump.h"
#include "SystemLogLibrary/SystemLogLibrary/CSystemLog.h"

class CNewLibrary
{
public:

	static void PrintNewInfo(void)
	{
		CNewLibrary::CCriticalSection srwLock(&CNewLibrary::mCriticalSection);

		for (auto& iter : mHeapHistory)
		{
			CSystemLog::GetInstance()->Log(TRUE, CSystemLog::eLogLevel::LogLevelError,L"NewLibrary", L"FileName : %s,  Line : %d ", iter.second->pFileName, iter.second->line);
		}

		return;
	}

private:

	static CNewLibrary* GetInstance(void)
	{
		static CNewLibrary newLibrary;

		return &newLibrary;
	}

	friend void* operator new(size_t size, const WCHAR* pFile, INT line);

	friend void* operator new[](size_t size, const WCHAR* pFile, INT line);

	friend void operator delete(void* p);

	friend void operator delete[](void* p);

	CNewLibrary(void)
	{
		InitializeCriticalSection(&mCriticalSection);

	}

	~CNewLibrary()
	{
	}

	class CCriticalSection
	{
	public:

		CCriticalSection(CRITICAL_SECTION* pSRWLock)
			:mpSRWLock(pSRWLock)
		{	
			EnterCriticalSection(mpSRWLock);
		}

		~CCriticalSection(void)
		{		
			LeaveCriticalSection(mpSRWLock);
		}

		CRITICAL_SECTION* mpSRWLock;
	};
	
	struct stHeapInfo
	{
		WCHAR* pFileName;
		INT line;
	};


	inline static CRITICAL_SECTION mCriticalSection;

	inline static std::unordered_map<void*, stHeapInfo*> mHeapHistory;
	//inline static CList<stHeapInfo*> mHeapHistory;
};

inline void* operator new(size_t size, const WCHAR* pFileName, INT line)
{
	CNewLibrary::GetInstance();

	CNewLibrary::stHeapInfo* pHeapInfo = (CNewLibrary::stHeapInfo*)malloc(sizeof(CNewLibrary::stHeapInfo));
	if (pHeapInfo == nullptr)
	{
		CCrashDump::Crash();

		return nullptr;
	}

	void* pRetval = malloc(size);
	
	pHeapInfo->pFileName = (WCHAR*)pFileName;

	pHeapInfo->line = line;

	CNewLibrary::CCriticalSection srwLock(&CNewLibrary::mCriticalSection);

	//CNewLibrary::mHeapHistory.PushBack(pHeapInfo);

	CNewLibrary::mHeapHistory.insert(std::make_pair(pRetval, pHeapInfo));

	return pRetval;
}

inline void* operator new[](size_t size, const WCHAR* pFileName, INT line)
{
	CNewLibrary::GetInstance();

	CNewLibrary::stHeapInfo* pHeapInfo = (CNewLibrary::stHeapInfo*)malloc(sizeof(CNewLibrary::stHeapInfo));
	if (pHeapInfo == nullptr)
	{
		CCrashDump::Crash();

		return nullptr;
	}

	void* pRetval = malloc(size);

	pHeapInfo->pFileName = (WCHAR*)pFileName;

	pHeapInfo->line = line;

	CNewLibrary::CCriticalSection srwLock(&CNewLibrary::mCriticalSection);

	//CNewLibrary::mHeapHistory.PushBack(pHeapInfo);

	CNewLibrary::mHeapHistory.insert(std::make_pair(pRetval, pHeapInfo));


	return pRetval;
}

inline void operator delete(void* pVoid)
{
	CNewLibrary::CCriticalSection srwLock(&CNewLibrary::mCriticalSection);

	auto iterE = CNewLibrary::mHeapHistory.end();

	//for (auto iter = CNewLibrary::mHeapHistory.begin(); iter != iterE; ++iter)
	//{
	//	if (iter->pointer == pVoid)
	//	{
	//		CNewLibrary::mHeapHistory.erase(iter);
	//	}
	//}

	auto iter = CNewLibrary::mHeapHistory.find(pVoid);

	if ( iter != iterE)
	{
		CNewLibrary::mHeapHistory.erase(iter->first);
	}

	free(pVoid);

	return;
}

inline void operator delete[](void* pVoid)
{	
	CNewLibrary::CCriticalSection srwLock(&CNewLibrary::mCriticalSection);

	auto iterE = CNewLibrary::mHeapHistory.end();

	//for (auto iter = CNewLibrary::mHeapHistory.begin(); iter != iterE; ++iter)
	//{
	//	if (iter->pointer == pVoid)
	//	{
	//		CNewLibrary::mHeapHistory.erase(iter);
	//	}
	//}

	auto iter = CNewLibrary::mHeapHistory.find(pVoid);

	if (iter != iterE)
	{
		CNewLibrary::mHeapHistory.erase(iter->first);
	}


	free(pVoid);

	return;
}


#define new new(__FILEW__, __LINE__)