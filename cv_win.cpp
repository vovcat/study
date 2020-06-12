// i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mwindows -static-libgcc cv_win.cpp -o cv_win.exe && wine ./cv_win.exe

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

extern "C" VOID WINAPI InitializeConditionVariable(PCONDITION_VARIABLE);
extern "C" BOOL WINAPI SleepConditionVariableCS(PCONDITION_VARIABLE,PCRITICAL_SECTION,DWORD);
extern "C" VOID WINAPI WakeAllConditionVariable(PCONDITION_VARIABLE);
extern "C" VOID WINAPI WakeConditionVariable(PCONDITION_VARIABLE);

#define BUFFER_SIZE 10
#define PRODUCER_SLEEP_TIME_MS 50
#define CONSUMER_SLEEP_TIME_MS 200

LONG Buffer[BUFFER_SIZE];
LONG LastItemProduced;
ULONG QueueSize;
ULONG QueueStartOffset;

CONDITION_VARIABLE BufferNotEmpty;
CONDITION_VARIABLE BufferNotFull;
CRITICAL_SECTION   BufferLock;

ULONG TotalItemsProduced;
ULONG TotalItemsConsumed;

BOOL StopRequested;

DWORD WINAPI ProducerThreadProc(PVOID p)
{
    ULONG ProducerId = (ULONG)(ULONG_PTR)p;

    while (true) {
        // Produce a new item.
        Sleep(rand() % PRODUCER_SLEEP_TIME_MS);
        ULONG Item = InterlockedIncrement(&LastItemProduced);

        EnterCriticalSection(&BufferLock);

        while (QueueSize == BUFFER_SIZE && StopRequested == FALSE) {
            // Buffer is full - sleep so consumers can get items.
            SleepConditionVariableCS(&BufferNotFull, &BufferLock, INFINITE);
        }

        if (StopRequested == TRUE) {
            LeaveCriticalSection(&BufferLock);
            break;
        }

        // Insert the item at the end of the queue and increment size.

        Buffer[(QueueStartOffset + QueueSize) % BUFFER_SIZE] = Item;
        QueueSize++;
        TotalItemsProduced++;

        printf("Producer %lu: item %2ld, queue size %2lu\r\n", ProducerId, Item, QueueSize);

        LeaveCriticalSection(&BufferLock);

        // If a consumer is waiting, wake it.
        WakeConditionVariable(&BufferNotEmpty);
    }

    printf("Producer %lu exiting\r\n", ProducerId);
    return 0;
}

DWORD WINAPI ConsumerThreadProc(PVOID p)
{
    ULONG ConsumerId = (ULONG)(ULONG_PTR)p;

    while (true) {
        EnterCriticalSection(&BufferLock);

        while (QueueSize == 0 && StopRequested == FALSE) {
            // Buffer is empty - sleep so producers can create items.
            SleepConditionVariableCS(&BufferNotEmpty, &BufferLock, INFINITE);
        }

        if (StopRequested == TRUE && QueueSize == 0) {
            LeaveCriticalSection(&BufferLock);
            break;
        }

        // Consume the first available item.
        LONG Item = Buffer[QueueStartOffset];
        QueueSize--;
        QueueStartOffset++;
        TotalItemsConsumed++;

        if (QueueStartOffset == BUFFER_SIZE) {
            QueueStartOffset = 0;
        }

        printf("Consumer %lu: item %2ld, queue size %2lu\r\n", ConsumerId, Item, QueueSize);

        LeaveCriticalSection(&BufferLock);

        // If a producer is waiting, wake it.
        WakeConditionVariable(&BufferNotFull);

        // Simulate processing of the item.
        Sleep(rand() % CONSUMER_SLEEP_TIME_MS);
    }

    printf("Consumer %lu exiting\r\n", ConsumerId);
    return 0;
}

int main( void )
{
    InitializeConditionVariable(&BufferNotEmpty);
    InitializeConditionVariable(&BufferNotFull);

    InitializeCriticalSection(&BufferLock);

    DWORD id;
    HANDLE hProducer1 = CreateThread(NULL, 0, ProducerThreadProc, (PVOID)1, 0, &id);
    HANDLE hConsumer1 = CreateThread(NULL, 0, ConsumerThreadProc, (PVOID)1, 0, &id);
    HANDLE hConsumer2 = CreateThread(NULL, 0, ConsumerThreadProc, (PVOID)2, 0, &id);

    puts("Press enter to stop...");
    getchar();

    EnterCriticalSection(&BufferLock);
    StopRequested = TRUE;
    LeaveCriticalSection(&BufferLock);

    WakeAllConditionVariable(&BufferNotFull);
    WakeAllConditionVariable(&BufferNotEmpty);

    WaitForSingleObject(hProducer1, INFINITE);
    WaitForSingleObject(hConsumer1, INFINITE);
    WaitForSingleObject(hConsumer2, INFINITE);

    printf("TotalItemsProduced: %lu, TotalItemsConsumed: %lu\r\n",
        TotalItemsProduced, TotalItemsConsumed);
    return 0;
}