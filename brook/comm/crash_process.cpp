#include "crash_process.h"

#ifdef _WIN32
#include   <windows.h>
#include <DbgHelp.h>
#include <string>

#pragma comment(lib, "Dbghelp.lib")

inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
{
	HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = NULL;

// 		MINIDUMP_CALLBACK_INFORMATION mci;
// 		mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
// 		mci.CallbackParam = 0;
		MINIDUMP_TYPE mdt = /*MiniDumpNormal;*/
							(MINIDUMP_TYPE)(MiniDumpWithFullMemory |
			MiniDumpWithFullMemoryInfo |
			MiniDumpWithHandleData |
			MiniDumpWithThreadInfo |
			MiniDumpWithUnloadedModules);
		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, NULL, /*&mci*/NULL);

		CloseHandle(hFile);
	}
}
static LPTOP_LEVEL_EXCEPTION_FILTER p = NULL;
long __stdcall handle_crash(PEXCEPTION_POINTERS pExceptionInfo)
{
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	std::string _exeName = szFilePath;
	std::string path = _exeName.substr(0, _exeName.rfind("\\"));
	_exeName = _exeName.substr(_exeName.rfind("\\")+1);
	_exeName = _exeName.substr(0,_exeName.rfind("."));
	_exeName = path + "\\" + _exeName + ".dmp";

	CreateMiniDump(pExceptionInfo, _exeName.c_str());

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

bool CRASH_PROC()
{
#ifdef _WIN32
	p = SetUnhandledExceptionFilter(handle_crash);
#else
	
#endif
	return true;
}
