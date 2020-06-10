// g++ -g -Og -Wall -Wextra test_cvq.cpp -o test_cvq -lpthread && ./test_cvq

#include <stdio.h> // printf(). getchar()
#include <unistd.h> // usleep()

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

template<typename T> struct nqueue
{
    std::queue<T> q;
    size_t size() const { return q.size(); }
    T get() { while (q.empty()); T r = q.front(); q.pop(); return r; }
    void put(const T &v) { q.push(v); }
};

template<typename T> struct cqueue
{
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable cv;
    typedef std::unique_lock<std::mutex> lock_t;
    size_t size() const { lock_t l(m); return q.size(); }
    T get() { lock_t l(m); while (q.empty()) cv.wait(l); T r = q.front(); q.pop(); return r; }
    void put(const T &v) { lock_t l(m); q.push(v); l.unlock(); cv.notify_one(); }
};

cqueue<int> getkey_q;

#define PRODUCER_SLEEP_TIME_MS 50
#define CONSUMER_SLEEP_TIME_MS 200

std::atomic<int> LastItemProduced;
std::atomic<int> TotalItemsProduced;
std::atomic<int> TotalItemsConsumed;
std::atomic<int> TotalOverflow;
bool StopRequested;

void ProducerThread(int ProducerId)
{
    while (true) {
        if (StopRequested) break;

        // Produce a new item.
        if (PRODUCER_SLEEP_TIME_MS) usleep(rand() % PRODUCER_SLEEP_TIME_MS * 1000);
        if (getkey_q.size() > 100000) { TotalOverflow++; continue; }

        // Insert the item at the end of the queue and increment size.
        int Item = ++LastItemProduced;
        getkey_q.put(Item);

        TotalItemsProduced++;

        if (PRODUCER_SLEEP_TIME_MS)
            printf("Producer %d: item %2d, queue size %2d\r\n", ProducerId, Item, getkey_q.size());
    }

    printf("Producer %d exiting\r\n", ProducerId);
}

void ConsumerThreadProc(int ConsumerId)
{
    while (true) {
        if (StopRequested && getkey_q.size() == 0)
            break;

        // Consume the first available item.
        int Item = getkey_q.get();
        TotalItemsConsumed++;

        if (CONSUMER_SLEEP_TIME_MS)
            printf("Consumer %d: item %2d, queue size %2d\r\n", ConsumerId, Item, getkey_q.size());

        // Simulate processing of the item.
        if (CONSUMER_SLEEP_TIME_MS) usleep(rand() % CONSUMER_SLEEP_TIME_MS * 1000);
    }

    printf("Consumer %d exiting\r\n", ConsumerId);
}

int main()
{
    std::thread Producer1(ProducerThread, 1);
    std::thread Consumer1(ConsumerThreadProc, 1);
    std::thread Consumer2(ConsumerThreadProc, 2);

    puts("Press enter to stop...");
    getchar();
    StopRequested = true;

    Producer1.join();
    Consumer1.join();
    Consumer2.join();

    printf("TotalItemsProduced: %d, TotalItemsConsumed: %d\r\n", TotalItemsProduced.load(), TotalItemsConsumed.load());
}
