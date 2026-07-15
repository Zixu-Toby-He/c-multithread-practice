#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 多线程库 */
#include <pthread.h>
/* 计时 */
#include <time.h>


unsigned char MaxThreadNumber = 8;
#ifdef _WIN32
	#include <windows.h>
	void setMaxThreadNumber()
	{
		SYSTEM_INFO systemInformation;
		GetSystemInfo(&systemInformation);
		MaxThreadNumber = systemInformation.dwNumberOfProcessors;
	}
#elif __linux__
	#include <unistd.h>
	#define setMaxThreadNumber() MaxThreadNumber=_SC_NPROCESSORS_CONF
#else
	#define setMaxThreadNumber() {}
#endif

#define pause() puts("");system("pause");puts("")

#define LN 1234567890

unsigned long long int add_from_m_to_n(unsigned long long int m, unsigned long long int n)
{
	if (m>n)
	{
		printf("Error in %s : m = %llu > n = %llu\n",__func__,m,n);
		pause();
		exit(1);
	}
	unsigned long long int sum = 0;
	for(unsigned long long int counting = m; counting <=n; counting++)
	{
		sum += counting;
	}
	return sum;
}

/* args = {m,n,sum}*/
void* threadMission(void* args)
{
	unsigned long long int m,n;
	m = *((unsigned long long int*)args);
	n = *(((unsigned long long int*)args) + 1);
	//printf("m = %llu, n = %llu\n",m,n);
	*(((unsigned long long int*)args) + 2) = add_from_m_to_n(m,n);
	return (void*)(((unsigned long long int*)args) + 2);
}

void test_oneThread()
{
	clock_t start ,end;
	start = clock();
	unsigned long long int sum = add_from_m_to_n(1,LN);
	end = clock();
	printf("Calculation done. 1 + 2 + 3 + ... + %llu = %llu. Costing %.05g s\n",LN,sum,(double)(end-start)/CLOCKS_PER_SEC);
}

void test_twoThreads()
{
	clock_t start ,end;
	
	start = clock();
	
	unsigned long long int mid = LN/2;
	pthread_t myThread_1 = NULL;
	pthread_t myThread_2 = NULL;
	unsigned long long int args_myThread_1[3] = {0,mid,0};
	unsigned long long int args_myThread_2[3] = {mid+1,LN,0};
	
	pthread_create(&myThread_1, NULL, threadMission, (void*)args_myThread_1);
	pthread_create(&myThread_2, NULL, threadMission, (void*)args_myThread_2);
	pthread_join(myThread_1,NULL);
	pthread_join(myThread_2,NULL);
	
	unsigned long long int sum = args_myThread_1[2] + args_myThread_2[2];
	end = clock();
	printf("Calculation done. 1 + 2 + 3 + ... + %llu = %llu. Costing %.05g s\n",LN,sum,(double)(end-start)/CLOCKS_PER_SEC);
}

void test_multiThreads(unsigned char threadnumber)
{
	clock_t start ,end;
	
	start = clock();
	
	unsigned long long int mid = LN/2;
	pthread_t* myThreads = (pthread_t*)malloc(threadnumber * sizeof(pthread_t));
	// memset(myThreads,0,threadnumber * sizeof(pthread_t));
	unsigned long long int* args_myThreads_data = (unsigned long long int*)malloc(3 * threadnumber * sizeof(unsigned long long int));
	for(size_t counting = 0;counting<threadnumber;counting++)
	{
		unsigned long long int n = (LN * (counting + 1))/threadnumber;
		unsigned long long int m = (LN * counting)/threadnumber + 1;
		args_myThreads_data[3 * counting + 0] = m;
		args_myThreads_data[3 * counting + 1] = n;
		args_myThreads_data[3 * counting + 2] = 0;
		//printf("m = %llu, n = %llu, sum = %llu\n",m,n,0);
		pthread_create(&myThreads[counting], NULL, threadMission, (void*)(args_myThreads_data + 3 * counting));
	}
	for(size_t counting = 0;counting<threadnumber;counting++)
	{
		pthread_join(myThreads[counting],NULL);
	}
	
	unsigned long long int sum = 0;
	for(size_t counting = 0;counting<threadnumber;counting++)
	{
		sum+=args_myThreads_data[3 * counting + 2];
	}
	end = clock();
	printf("Calculation done. 1 + 2 + 3 + ... + %llu = %llu. Costing %.05g s\n",LN,sum,(double)(end-start)/CLOCKS_PER_SEC);
}


int main()
{
	printf("Multithread Test\n"
	       "\n"
	       "Mission: calculate 1 + 2 + 3 + ... + %llu = ?\n",LN);
	puts("---------------------------------------------");
	puts("One thread");
	test_oneThread();
	puts("---------------------------------------------");
	puts("Two threads");
	test_twoThreads();
	puts("---------------------------------------------");
	setMaxThreadNumber();
	printf("Multi threads(%u)\n",(unsigned int)MaxThreadNumber);
	test_multiThreads(MaxThreadNumber);
	puts("---------------------------------------------");
	pause();
	return 0;
}