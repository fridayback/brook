// 这个文件用于定义一些和平台相关的参数和宏

#ifndef _TS_AP_COMMON_CONFIG_H_
#define _TS_AP_COMMON_CONFIG_H_


// 操作系统
#ifdef _WIN32

//  win32 平台
#define TS_AP_OS_WIN32 // Win32

#define STDCALL __stdcall
typedef __int64 INT64;

#elif defined(_WIN64)

#define STDCALL __stdcall
typedef __int64 INT64;

//  win32 平台
#define TS_AP_WIN64 // Win64

#else

#define TS_AP_LINUX//	linux

#define STDCALL 
typedef long long INT64;


#endif // Win32,Win64



#if (defined(TS_AP_OS_WIN32) || defined(TS_AP_WIN64))

#define TS_AP_WINDOWS // Windows

#endif


#endif // !defined(_TS_AP_COMMON_CONFIG_H_)
//
//#elif defined(__linux)
//#define TSM_OS_LINUX32
//#elif defined(__unix)
//#define TSM_OS_UNIX32
//#else
//#error "OS Code"
//#endif
