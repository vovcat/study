// i686-w64-mingw32-g++ -g -Og -Wall -Wextra -mwindows -static-libgcc win_cvq.cpp -o win_cvq && wine ./win_cvq

#include <windows.h>
#include <stdio.h> // printf(), getchar()

#include <queue>

extern "C" VOID WINAPI InitializeConditionVariable(PCONDITION_VARIABLE);
extern "C" BOOL WINAPI SleepConditionVariableCS(PCONDITION_VARIABLE,PCRITICAL_SECTION,DWORD);
extern "C" VOID WINAPI WakeAllConditionVariable(PCONDITION_VARIABLE);
extern "C" VOID WINAPI WakeConditionVariable(PCONDITION_VARIABLE);

CRITICAL_SECTION getkey_q_mutex;
CONDITION_VARIABLE getkey_q_notempty;
std::queue<int> getkey_q;

void getkey_q_init()
{
    InitializeConditionVariable(&getkey_q_notempty);
    InitializeCriticalSection(&getkey_q_mutex);
}

int getkey_q_get()
{
    EnterCriticalSection(&getkey_q_mutex);
    while (getkey_q.size() == 0) SleepConditionVariableCS(&getkey_q_notempty, &getkey_q_mutex, INFINITE);
    int key = getkey_q.front(); getkey_q.pop();
    LeaveCriticalSection(&getkey_q_mutex);
    return key;
}

int getkey_q_size()
{
    EnterCriticalSection(&getkey_q_mutex);
    unsigned size = getkey_q.size();
    LeaveCriticalSection(&getkey_q_mutex);
    return size;
}

void getkey_q_put(int key)
{
    EnterCriticalSection(&getkey_q_mutex);
    getkey_q.push(key);
    LeaveCriticalSection(&getkey_q_mutex);
    WakeConditionVariable(&getkey_q_notempty);
}


#define PRODUCER_SLEEP_TIME_MS 50
#define CONSUMER_SLEEP_TIME_MS 200

LONG LastItemProduced;
LONG TotalItemsProduced;
LONG TotalItemsConsumed;
LONG TotalOverflow;

BOOL StopRequested;

DWORD WINAPI ProducerThreadProc(PVOID p)
{
    ULONG ProducerId = (ULONG)(ULONG_PTR)p;

    while (true) {
        if (StopRequested) break;

        // Produce a new item.
        if (PRODUCER_SLEEP_TIME_MS) Sleep(rand() % PRODUCER_SLEEP_TIME_MS);
        if (getkey_q_size() > 100000) { TotalOverflow++; continue; }

        // Insert the item at the end of the queue and increment size.
        LONG Item = InterlockedIncrement(&LastItemProduced);
        getkey_q_put((int)Item);

        TotalItemsProduced++;

        if (PRODUCER_SLEEP_TIME_MS)
            printf("Producer %ld: item %2ld, queue size %2d\r\n", ProducerId, Item, getkey_q_size());
    }

    printf("Producer %lu exiting\r\n", ProducerId);
    return 0;
}

DWORD WINAPI ConsumerThreadProc(PVOID p)
{
    ULONG ConsumerId = (ULONG)(ULONG_PTR)p;

    while (true) {
        if (StopRequested && getkey_q_size() == 0)
            break;

        // Consume the first available item.
        LONG Item = (LONG) getkey_q_get();
        InterlockedIncrement(&TotalItemsConsumed);

        if (CONSUMER_SLEEP_TIME_MS)
            printf("Consumer %lu: item %2ld, queue size %2d\r\n", ConsumerId, Item, getkey_q_size());

        // Simulate processing of the item.
        if (CONSUMER_SLEEP_TIME_MS) Sleep(rand() % CONSUMER_SLEEP_TIME_MS);
    }

    printf("Consumer %lu exiting\r\n", ConsumerId);
    return 0;
}

int main()
{
    getkey_q_init();

    DWORD id;
    HANDLE hProducer1 = CreateThread(NULL, 0, ProducerThreadProc, (PVOID)1, 0, &id);
    HANDLE hConsumer1 = CreateThread(NULL, 0, ConsumerThreadProc, (PVOID)1, 0, &id);
    HANDLE hConsumer2 = CreateThread(NULL, 0, ConsumerThreadProc, (PVOID)2, 0, &id);

    puts("Press enter to stop...");
    getchar();
    StopRequested = TRUE;

    WaitForSingleObject(hProducer1, INFINITE);
    WaitForSingleObject(hConsumer1, INFINITE);
    WaitForSingleObject(hConsumer2, INFINITE);

    printf("TotalItemsProduced: %lu, TotalItemsConsumed: %lu\r\n", TotalItemsProduced, TotalItemsConsumed);
}
