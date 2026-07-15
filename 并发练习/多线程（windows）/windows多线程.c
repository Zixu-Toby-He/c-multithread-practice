#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 多线程库 */
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
/* 计时 */
#include <time.h>

// 最大线程数
unsigned char MaxThreadNumber = 8;
#ifdef _WIN32
// 获取系统信息以设置最大线程数
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

// 计算范围
#define LN ((unsigned long long int)1234567890)

// 从m加到n的函数
unsigned long long int add_from_m_to_n(unsigned long long int m, unsigned long long int n)
{
    if (m > n)
    {
        printf("Error in %s : m = %llu > n = %llu\n", __func__, m, n);
        pause();
        exit(1);
    }
    unsigned long long int sum = 0;
    for (unsigned long long int counting = m; counting <= n; counting++)
    {
        sum += counting;
    }
    return sum;
}

/* args = { m,n,sum }*/
#ifdef _WIN32
// 线程任务函数
// windows 利用
DWORD WINAPI threadMission(LPVOID args)
{
    unsigned long long int m, n;
    m = *((unsigned long long int*)args);
    n = *(((unsigned long long int*)args) + 1);
    //printf("m = %llu, n = %llu\n",m,n);
    *(((unsigned long long int*)args) + 2) = add_from_m_to_n(m, n);
    return 0;
}
#else
void* threadMission(void* args)
{
    unsigned long long int m, n;
    m = *((unsigned long long int*)args);
    n = *(((unsigned long long int*)args) + 1);
    //printf("m = %llu, n = %llu\n",m,n);
    *(((unsigned long long int*)args) + 2) = add_from_m_to_n(m, n);
    return (void*)(((unsigned long long int*)args) + 2);
}
#endif

// 单线程测试函数
void test_oneThread()
{
    clock_t start, end;
    start = clock();
    unsigned long long int sum = add_from_m_to_n(1, LN);
    end = clock();
    printf("Calculation done. 1 + 2 + 3 + ... + %llu = %llu. Costing %.05g s\n", (unsigned long long int)LN, sum, (double)(end - start) / CLOCKS_PER_SEC);
}

// 双线程测试函数
void test_twoThreads()
{
    clock_t start, end;

    start = clock();

    unsigned long long int mid = LN / 2;
    // 声明线程变量
#ifdef _WIN32
    // windows.h 中利用 HANDLE 创建线程
    HANDLE myThread_1 = NULL;
    HANDLE myThread_2 = NULL;
#else
    pthread_t myThread_1 = NULL;
    pthread_t myThread_2 = NULL;
#endif
    unsigned long long int args_myThread_1[3] = { 0, mid, 0 };
    unsigned long long int args_myThread_2[3] = { mid + 1, LN, 0 };

    // 创建线程并运行
#ifdef _WIN32
    // 创建线程
    myThread_1 = CreateThread(NULL, 0, threadMission, (void*)args_myThread_1, 0, NULL);
    myThread_2 = CreateThread(NULL, 0, threadMission, (void*)args_myThread_2, 0, NULL);
    if ((myThread_1 == 0) || (myThread_2 == 0))
    {
        printf("Error in %s:\n", __func__);
        puts("\tFail creating threads.");
        pause();
        exit(EXIT_FAILURE);
    }
    // 等待线程完成
    WaitForSingleObject(myThread_1, INFINITE);
    WaitForSingleObject(myThread_2, INFINITE);
    // 关闭线程
    CloseHandle(myThread_1);
    CloseHandle(myThread_2);
#else
    pthread_create(&myThread_1, NULL, threadMission, (void*)args_myThread_1);
    pthread_create(&myThread_2, NULL, threadMission, (void*)args_myThread_2);
    pthread_join(myThread_1, NULL);
    pthread_join(myThread_2, NULL);
#endif

    unsigned long long int sum = args_myThread_1[2] + args_myThread_2[2];
    end = clock();
    printf("Calculation done. 1 + 2 + 3 + ... + %llu = %llu. Costing %.05g s\n", LN, sum, (double)(end - start) / CLOCKS_PER_SEC);
}

// 多线程测试函数
void test_multiThreads(unsigned char threadnumber)
{
    clock_t start, end;

    start = clock();

    unsigned long long int mid = LN / 2;
#ifdef _WIN32
    HANDLE* myThreads = (HANDLE*)malloc(threadnumber * sizeof(HANDLE));
#else
    pthread_t* myThreads = (pthread_t*)malloc(threadnumber * sizeof(pthread_t));
#endif
    if (myThreads == NULL)
    {
        printf("Error in %s:\n\tmalloc for myThreads failed.", __func__);
        exit(EXIT_FAILURE);
    }
    unsigned long long int* args_myThreads_data = (unsigned long long int*)malloc(3 * threadnumber * sizeof(unsigned long long int));
    if (args_myThreads_data == NULL)
    {
        printf("Error in %s:\n\tmalloc for args_myThreads_data failed.",__func__);
        exit(EXIT_FAILURE);
    }
    for (size_t counting = 0; counting < threadnumber; counting++)
    {
        unsigned long long int n = (LN * (counting + 1)) / threadnumber;
        unsigned long long int m = (LN * counting) / threadnumber + 1;
        args_myThreads_data[3 * counting + 0] = m;
        args_myThreads_data[3 * counting + 1] = n;
        args_myThreads_data[3 * counting + 2] = 0;
        //printf("m = %llu, n = %llu, sum = %llu\n",m,n,0);
#ifdef _WIN32
        // 创建线程
        myThreads[counting] = CreateThread(NULL, 0, threadMission, (void*)(args_myThreads_data + 3 * counting), 0, NULL);
#else
        pthread_create(&myThreads[counting], NULL, threadMission, (void*)(args_myThreads_data + 3 * counting));
#endif
    }
    for (size_t counting = 0; counting < threadnumber; counting++)
    {
#ifdef _WIN32
        // 等待线程完成
        WaitForSingleObject(myThreads[counting], INFINITE);
        // 关闭线程句柄
        CloseHandle(myThreads[counting]);
#else
        pthread_join(myThreads[counting], NULL);
#endif
    }

    unsigned long long int sum = 0;
    for (size_t counting = 0; counting < threadnumber; counting++)
    {
        sum += args_myThreads_data[3 * counting + 2];
    }
    end = clock();
    printf("Calculation done. 1 + 2 + 3 + ... + %llu = %llu. Costing %.05g s\n", LN, sum, (double)(end - start) / CLOCKS_PER_SEC);

    free(myThreads);
    free(args_myThreads_data);
}

int main()
{
    printf("Multithread Test\n"
        "\n"
        "Mission: calculate 1 + 2 + 3 + ... + %llu = ?\n", LN);
    puts("---------------------------------------------");
    puts("One thread");
    test_oneThread();
    puts("---------------------------------------------");
    puts("Two threads");
    test_twoThreads();
    puts("---------------------------------------------");
    setMaxThreadNumber();
    printf("Multi threads(%u)\n", (unsigned int)MaxThreadNumber);
    test_multiThreads(MaxThreadNumber);
    puts("---------------------------------------------");
    pause();
    return 0;
}
