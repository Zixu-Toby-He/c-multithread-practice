#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 多线程库 */
#include "Threads.h"

/* 计时 */
#include <time.h>

// 最大线程数
unsigned char MaxThreadNumber = 8;

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
defineThreadFunc(threadMission)
{
    unsigned long long int m = *((unsigned long long int*)args);
    unsigned long long int n = *((unsigned long long int*)args + 1);
    *((unsigned long long int*)args + 2) = add_from_m_to_n(m, n);
    return 0;
}

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
    threadtype myThread_1 = (threadtype)0;
    threadtype myThread_2 = (threadtype)0;
    unsigned long long int args_myThread_1[3] = { 0, mid, 0 };
    unsigned long long int args_myThread_2[3] = { mid + 1, LN, 0 };
    // 创建线程并运行
    threadTool_creat(&myThread_1, threadMission, args_myThread_1);
    threadTool_creat(&myThread_2, threadMission, args_myThread_2);
    if ((myThread_1 == 0) || (myThread_2 == 0))
    {
        printf("Error in %s:\n", __func__);
        puts("\tFail creating threads.");
        pause();
        exit(EXIT_FAILURE);
    }
    // 等待线程完成
    threadTool_join(myThread_1);
    threadTool_join(myThread_2);
    threadTool_end(myThread_1);
    threadTool_end(myThread_2);

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
    threadtype* myThreads = (threadtype*)malloc(threadnumber * sizeof(threadtype));
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
        threadTool_creat(&myThreads[counting],threadMission, (void*)(args_myThreads_data + 3 * counting));
    }
    for (size_t counting = 0; counting < threadnumber; counting++)
    {
        threadTool_join(myThreads[counting]);
    }
    for (size_t counting = 0; counting < threadnumber; counting++)
    {
        threadTool_end(myThreads[counting]);
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
    MaxThreadNumber = (unsigned char)getCoreNumber();
    printf("Multi threads(%u)\n", (unsigned int)MaxThreadNumber);
    test_multiThreads(MaxThreadNumber);
    puts("---------------------------------------------");
    pause();
    return 0;
}
