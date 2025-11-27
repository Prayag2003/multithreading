#include <iostream>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>
typedef long int ull;
using namespace std;

/**
 * std::async - Beginner Notes
 *
 * - std::async is a high-level way to run a function in the background and get its result later.
 * - It returns a std::future<T> that you can use to get the result (blocks if not ready).
 * - Under the hood, it's a wrapper over promise/future, but much easier to use for simple cases.
 *
 * Launch policies:
 *   - std::launch::async: Always runs the function in a new thread.
 *   - std::launch::deferred: Runs the function only when you call get() or wait() (in the same thread, not concurrent).
 *   - std::launch::async | std::launch::deferred: Lets the implementation choose (may or may not create a thread).
 *
 * Usage tips:
 *   - Use std::async for simple parallel tasks where you want the result later.
 *   - If you use std::launch::deferred, the function won't run until you ask for the result (no parallelism).
 *   - If you use std::launch::async, the function runs right away in a new thread.
 *   - The returned future's get() will block until the result is ready.
 *   - If the function throws, get() will rethrow the exception in the caller.
 */

ull findOddSum(ull start, ull end)
{
    cout << "OddSum Thread ID: " << std::this_thread::get_id() << "\n";
    ull OddSum = 0;
    for (ull i = start; i <= end; i++)
    {
        if (i & 1)
            OddSum += i;
    }
    return OddSum;
}

int main()
{
    ull start = 0, end = 1900000000;
    cout << "Main Thread ID: " << std::this_thread::get_id() << "\n";

    // 1: deferred (lazy) policy (runs in main thread, not parallel)
    std::future<ull> oddSum = std::async(std::launch::deferred, findOddSum, start, end);
    cout << "[deferred] Thread will run only when we call get().\n";
    cout << "Waiting for result!!" << "\n";
    cout << "OddSum (deferred): " << oddSum.get() << "\n";

    // 2: async (eager) policy (runs in a new thread, parallel)
    std::future<ull> oddSumAsync = std::async(std::launch::async, findOddSum, start, end);
    cout << "[async] Thread created immediately.\n";
    cout << "Waiting for async result!!\n";
    cout << "OddSum (async): " << oddSumAsync.get() << "\n";

    cout << "Completed!\n";
    return 0;
}
