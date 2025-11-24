# ThreadPool

A small C++ learning repository demonstrating threading primitives and synchronization techniques using the C++ Standard Library (`std::thread`, `std::mutex`, condition variables, and related utilities). The examples live under `pre-requisites/basics` and are meant as bite-sized exercises for understanding common concurrency concepts and pitfalls.

**Goals**

- Demonstrate thread creation and lifecycle.
- Show common concurrency problems (race conditions) and fixes.
- Illustrate mutex usage, try-lock variants, lock wrappers, and condition variables.

**How to build**

Create a `bin` directory to place executables (optional):

```
mkdir -p bin
```

To compile all examples quickly from the shell you can run:

```
for f in pre-requisites/basics/*.cpp; do
	name=$(basename "$f" .cpp)
	g++ -std=c++17 -Wall -Wextra -pthread "$f" -o "bin/$name"
done
```

Run an example:

```
./bin/00_basics_of_threads
```

**Examples (files and short descriptions)**

- **`00_basics_of_threads.cpp`**: Beginner introduction to `std::thread`.

     - Intent: Show how to start threads and the simplest way to run work concurrently.
     - What to look for: Race conditions when multiple threads write to `std::cout` at the same time.
     - How to run: Compile and run the program; you may see interleaved/garbled output when threads print simultaneously.
     - Key takeaways: Threads run independently; shared resources require synchronization.
     - Suggested exercise: Add sleeps to force interleaving, then protect `std::cout` with a mutex and compare output.

- **`01_thread_creation.cpp`**: Different thread creation idioms.

     - Intent: Demonstrate starting threads from functions, functors, and lambdas and passing arguments safely.
     - What to look for: How arguments are copied or moved into the thread and how to use `std::ref` for references.
     - How to run: Compile and run; each thread should perform its assigned work and print identifiable messages.
     - Key takeaways: Prefer passing small PODs by value, use `std::ref` for references, and ensure referenced data outlives the thread.
     - Suggested exercise: Create a shared vector and attempt to modify it from multiple threads (then fix with a mutex).

- **`02_join_detach.cpp`**: Thread lifetime and ownership.

     - Intent: Explain `join()` vs `detach()` and when to use each.
     - What to look for: Program termination behavior — with `join()` the main thread waits; with `detach()` threads run independently.
     - How to run: Try both modes; observe program exit sometimes before detached threads finish (or crash if they access invalid memory).
     - Key takeaways: Use `join()` for predictable cleanup; `detach()` requires caution and stable resources.
     - Suggested exercise: Convert a `join()` example to `detach()` and add a bug (dangling reference) to see failure modes.

- **`03_mutex.cpp`**: Protecting shared data with `std::mutex`.

     - Intent: Introduce `std::mutex` for mutual exclusion around shared state.
     - What to look for: Critical sections where shared counters or I/O are updated.
     - How to run: Run a version without mutex (observe race), then enable the mutex and compare deterministic results.
     - Key takeaways: Locks serialize access but can introduce contention; keep critical sections short.
     - Suggested exercise: Measure timing differences with and without mutex protection.

- **`04_try_lock.cpp`**: Non-blocking lock attempts.

     - Intent: Show how `try_lock()` lets a thread attempt to acquire a lock without waiting.
     - What to look for: Code paths executed when the lock is busy vs when it's acquired.
     - How to run: Run multiple threads competing for a mutex; observe how some threads back off or retry.
     - Key takeaways: `try_lock()` is useful to avoid deadlocks or to perform alternative work if a resource is busy.
     - Suggested exercise: Implement exponential backoff when `try_lock()` fails.

- **`05_multiple_try_lock.cpp`**: Locking multiple mutexes safely.

     - Intent: Demonstrate strategies to lock more than one mutex without deadlock (e.g., `std::lock`, consistent ordering).
     - What to look for: Potential deadlock scenarios and how the example avoids them.
     - How to run: Run two threads that need two resources; observe correct behavior when using safe locking patterns.
     - Key takeaways: Never assume independent locks are safe; either adopt an order or use `std::lock`/scoped wrappers.
     - Suggested exercise: Intentionally introduce inverse lock ordering to see a deadlock, then fix it.

- **`06_recursion_lock.cpp`**: Recursive mutexes and reentrancy.

     - Intent: Explain `std::recursive_mutex` and when recursion in locks is required.
     - What to look for: Functions that call each other and attempt to lock the same mutex multiple times.
     - How to run: Compare behavior with `std::mutex` (would deadlock) vs `std::recursive_mutex` (allows reentrant locking).
     - Key takeaways: Prefer non-recursive locks when possible; recursive locks can hide design issues.
     - Suggested exercise: Refactor recursive locking into a design that avoids recursion.

- **`07_lock_guard.cpp`**: RAII locking with `std::lock_guard`.

     - Intent: Use RAII to automatically release locks and make code exception-safe.
     - What to look for: Scope-based locking and guaranteed unlocking at scope exit.
     - How to run: Introduce an exception in a locked section and see that the lock is still released.
     - Key takeaways: Prefer `std::lock_guard` for simple lock/unlock needs to avoid forgetting unlocks.
     - Suggested exercise: Replace manual `lock()`/`unlock()` calls with `std::lock_guard` in an example.

- **`08_unique_lock.cpp`**: Flexible locking with `std::unique_lock`.

     - Intent: Show a more flexible lock type that supports deferred locking, timed waits, and transfer of ownership.
     - What to look for: Examples of `unique_lock` being unlocked and re-locked, or used with condition variables.
     - How to run: Explore deferred lock and timed lock examples to see different behaviors.
     - Key takeaways: `std::unique_lock` is slightly heavier than `lock_guard` but necessary for advanced patterns.
     - Suggested exercise: Replace `lock_guard` with `unique_lock` to allow calling `wait()` on a condition variable.

- **`09_conditional_variable.cpp`**: Synchronization with `std::condition_variable`.
     - Intent: Demonstrate producer/consumer coordination using a condition variable to wait and notify.
     - What to look for: Proper use of `std::unique_lock`, predicate usage with `wait()`, and using `notify_one()` / `notify_all()`.
     - How to run: Start producer and consumer threads; watch consumers block until producers push data and notify.
     - Key takeaways: Always use a predicate with `wait()` to avoid spurious wakeups; protect shared data with a mutex.
     - Suggested exercise: Implement a bounded buffer and make multiple producers/consumers.
