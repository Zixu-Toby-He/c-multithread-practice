#include "Threads.h"

#ifdef _WIN32

size_t getCoreNumber()
{
	SYSTEM_INFO systemInformation;
	GetSystemInfo(&systemInformation);
	return systemInformation.dwNumberOfProcessors;
}

/* linux*/
#else
#endif

void threadTool_creat(threadtype* thread, threadfunc missionFunction, argstype args)
{
	#ifdef _WIN32
		*thread = CreateThread(NULL, 0, missionFunction, args, 0, NULL);
	/* linux*/
	#else
		pthread_create(thread, NULL, missionFunction, args);
	#endif
}
void threadTool_join(threadtype thread)
{
	#ifdef _WIN32
		WaitForSingleObject(thread, INFINITE);
	/* linux*/
	#else
		pthread_join(thread, NULL);
	#endif
}
void threadTool_end(threadtype thread)
{
	#ifdef _WIN32
		CloseHandle(thread);
	/* linux*/
	#else
		pthread_join(thread, NULL);
	#endif
}