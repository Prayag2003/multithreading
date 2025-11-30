#include <iostream>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>

// NOTE: this file is an educational example for `std::promise` and `std::future`.
// It shows how one thread can compute a value and communicate it back to the
// main thread via a promise/future pair.

using namespace std;
typedef long int ull; // kept as-is from the original example (named 'ull' here)

/*
 * Quick reference:
 *  - std::promise<T> lets a producer set a value (or an exception) at some point.
 *  - std::future<T> obtained from the promise lets a consumer wait for and
 *    retrieve that value. Calling `get()` blocks until the value is available.
 *
 * Important notes for beginners:
 *  - `promise` must be moved into the thread if it's passed by value (promises are
 *    not copyable but are movable). We use `std::move` below.
 *  - `future::get()` can only be called once on the same future; after `get()` it
 *    transfers the stored value out and the future becomes invalid.
 *  - If the producer sets an exception via `promise.set_exception(...)`, then
 *    calling `future.get()` will rethrow that exception in the consumer thread.
 *  - Alternative: `std::async` with `std::launch::async` returns a future and
 *    is often simpler for fire-and-forget computations.
 */

// Compute the sum of odd numbers in the range [start, end] and set it on the promise.
// The promise is passed as an rvalue reference to emphasize that it is moved into
// the worker thread and will be used there to communicate the result.
void findOddSum(promise<ull> &&OddSumPromise, ull start, ull end)
{
    ull OddSum = 0;
    for (ull i = start; i <= end; i++)
    {
        // check lowest bit to determine oddness; equivalent to (i % 2 != 0)
        if (i & 1)
            OddSum += i;
    }

    // Set the value on the promise. This unblocks any thread waiting on the future.
    OddSumPromise.set_value(OddSum);
    // If something went wrong you could call:
    // OddSumPromise.set_exception(std::make_exception_ptr(std::runtime_error("fail")));
}

int main()
{
    // Small note: the chosen range is large on purpose to demonstrate a non-trivial
    // background computation; adjust `end` to a smaller number when experimenting.
    ull start = 0, end = 1900000000;

    // Create a promise and obtain its future. The future will be used to retrieve
    // the result once the worker thread sets it.
    promise<ull> OddSumPromise;
    future<ull> OddSumFuture = OddSumPromise.get_future();

    cout << "Thread created!!" << "\n";

    // Move the promise into the thread (promises are movable but not copyable).
    // The worker will call set_value() on the moved promise when ready.
    thread t1(findOddSum, std::move(OddSumPromise), start, end);

    // Example: if you want the main thread to do other work while the worker runs,
    // you could poll check the future with wait_for or wait_until. Here we simply join.
    if (t1.joinable())
        t1.join();

    cout << "Waiting for results...\n";

    // future::get() will block until the promise sets a value. It will also rethrow
    // any exception the producer stored via set_exception().
    try
    {
        auto t0 = chrono::steady_clock::now();
        ull result = OddSumFuture.get();
        auto t1_time = chrono::steady_clock::now();
        chrono::duration<double> elapsed = t1_time - t0;

        cout << "Value returned from the odd sum: " << result << "\n";
        cout << "(retrieval took " << elapsed.count() << " seconds)\n";
    }
    catch (const exception &ex)
    {
        // If the worker called set_exception, we receive that exception here.
        cerr << "Error while getting result: " << ex.what() << "\n";
    }
}