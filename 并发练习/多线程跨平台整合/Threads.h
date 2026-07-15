#ifndef __TOOLS_THREADS_H__
#define __TOOLS_THREADS_H__

/* windows 操作系统*/
#ifdef _WIN32

	#include <windows.h>
	// 获取核心数量
	size_t getCoreNumber();
	typedef HANDLE threadtype ;
	typedef LPVOID argstype   ;
	typedef DWORD WINAPI threadfunc(LPVOID);
	#define defineThreadFunc(funcname) DWORD WINAPI funcname(LPVOID args)
/* linux 操作系统*/
#else
	#include <pthread.h>
	#include <unistd.h>
	#define getCoreNumber() (_SC_NPROCESSORS_CONF)
	typedef  pthread_t threadtype;
	typedef  void*     argstype  ;
	typedef  void*     threadfunc(void*);
	#define defineThreadFunc(funcname) void* funcname(void* args)
#endif

void threadTool_creat(threadtype* thread, threadfunc missionFunction, argstype args);
void threadTool_join(threadtype thread);
void threadTool_end(threadtype thread);


#endif
