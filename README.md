# C++ Threading & Synchronization Learning Guide 🧵

A comprehensive learning repository for C++ concurrency concepts using `std::thread`, `std::mutex`, condition variables, and related utilities.

## Quick Start

### Build All Examples

```bash
mkdir -p bin
for f in pre-requisites/basics/*.cpp; do
    name=$(basename "$f" .cpp)
    g++ -std=c++17 -Wall -Wextra -pthread "$f" -o "bin/$name"
done

for f in pre-requisites/implementations/*.cpp; do
    name=$(basename "$f" .cpp)
    g++ -std=c++17 -Wall -Wextra -pthread "$f" -o "bin/$name"
done
```

### Run an Example

```bash
./bin/00_basics_of_threads
```

---

## Basics: Threading Fundamentals

Learn core threading concepts step by step.

| #   | Code File                                                                          | Documentation                                                                     | What You'll Learn                                           |
| --- | ---------------------------------------------------------------------------------- | --------------------------------------------------------------------------------- | ----------------------------------------------------------- |
| 00  | [`00_basics_of_threads.cpp`](pre-requisites/basics/00_basics_of_threads.cpp)       | [📖 Basics of Threads](pre-requisites/documentation/00_basics_of_threads.md)      | What threads are, why they're useful, race conditions       |
| 01  | [`01_thread_creation.cpp`](pre-requisites/basics/01_thread_creation.cpp)           | [📖 Thread Creation](pre-requisites/documentation/01_thread_creation.md)          | Functions, functors, lambdas, passing arguments, `std::ref` |
| 02  | [`02_join_detach.cpp`](pre-requisites/basics/02_join_detach.cpp)                   | [📖 join() vs detach()](pre-requisites/documentation/02_join_detach.md)           | Thread lifetime, waiting, independent threads               |
| 03  | [`03_mutex.cpp`](pre-requisites/basics/03_mutex.cpp)                               | [📖 Mutex Protection](pre-requisites/documentation/03_mutex.md)                   | Protecting shared data, critical sections, locks            |
| 04  | [`04_try_lock.cpp`](pre-requisites/basics/04_try_lock.cpp)                         | [📖 try_lock()](pre-requisites/documentation/04_try_lock.md)                      | Non-blocking lock attempts, avoiding deadlocks              |
| 05  | [`05_multiple_try_lock.cpp`](pre-requisites/basics/05_multiple_try_lock.cpp)       | [📖 Multiple Locks](pre-requisites/documentation/05_multiple_try_lock.md)         | Locking multiple mutexes safely, `std::lock()`              |
| 06  | [`06_recursion_lock.cpp`](pre-requisites/basics/06_recursion_lock.cpp)             | [📖 Recursive Mutex](pre-requisites/documentation/06_recursion_lock.md)           | Recursive locking, reentrancy, `std::recursive_mutex`       |
| 07  | [`07_lock_guard.cpp`](pre-requisites/basics/07_lock_guard.cpp)                     | [📖 Lock Guard](pre-requisites/documentation/07_lock_guard.md)                    | RAII pattern, automatic unlocking, exception safety         |
| 08  | [`08_unique_lock.cpp`](pre-requisites/basics/08_unique_lock.cpp)                   | [📖 Unique Lock](pre-requisites/documentation/08_unique_lock.md)                  | Manual control, deferred locking, timed waits               |
| 09  | [`09_conditional_variable.cpp`](pre-requisites/basics/09_conditional_variable.cpp) | [📖 Condition Variables](pre-requisites/documentation/09_conditional_variable.md) | Thread signaling, producer-consumer, `wait()` / `notify()`  |
| 10  | [`10_deadlock.cpp`](pre-requisites/basics/10_deadlock.cpp)                         | [📖 Deadlock Prevention](pre-requisites/documentation/10_deadlock.md)             | Circular waits, safe lock ordering, detection               |
| 11  | [`11_future_and_promise.cpp`](pre-requisites/basics/11_future_and_promise.cpp)     | [📖 Futures & Promises](pre-requisites/documentation/11_future_and_promise.md)    | Getting return values from threads, exception passing       |
| 12  | [`12_async.cpp`](pre-requisites/basics/12_async.cpp)                               | [📖 std::async](pre-requisites/documentation/12_async.md)                         | Simplest concurrency, automatic thread management           |

---

## Implementations: Real-World Patterns

Apply concepts to solve practical problems.

| #   | Code File                                                                                                 | Documentation                                                                                      | What You'll Learn                                |
| --- | --------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------- | ------------------------------------------------ |
| 00  | [`00_producer_consumer_mutex.cpp`](pre-requisites/implementations/00_producer_consumer_mutex.cpp)         | [📖 Producer-Consumer (Mutex)](pre-requisites/documentation/00_producer_consumer_mutex.md)         | Bounded buffer pattern, mutex-based coordination |
| 01  | [`01_atomic.cpp`](pre-requisites/implementations/01_atomic.cpp)                                           | [📖 Atomics](pre-requisites/documentation/01_atomic.md)                                            | Lock-free synchronization, atomic operations     |
| 02  | [`02_binary_semaphore.cpp`](pre-requisites/implementations/02_binary_semaphore.cpp)                       | [📖 Binary Semaphore](pre-requisites/documentation/02_binary_semaphore.md)                         | Semaphore basics, acquire/release pattern        |
| 03  | [`03_producer_consumer_semaphore.cpp`](pre-requisites/implementations/03_producer_consumer_semaphore.cpp) | [📖 Producer-Consumer (Semaphore)](pre-requisites/documentation/03_producer_consumer_semaphore.md) | Elegant bounded buffer with semaphores           |

---

## Key Concepts Quick Reference

| Concept                 | Use When                                   | See Example                          |
| ----------------------- | ------------------------------------------ | ------------------------------------ |
| **Mutex**               | Protecting shared data                     | `03_mutex.cpp`                       |
| **Lock Guard**          | Simple RAII locking                        | `07_lock_guard.cpp`                  |
| **Unique Lock**         | Need manual control or condition variables | `08_unique_lock.cpp`                 |
| **Condition Variables** | Threads need to signal each other          | `09_conditional_variable.cpp`        |
| **std::async**          | Need results from threads                  | `12_async.cpp`                       |
| **Atomics**             | Simple shared flags/counters               | `01_atomic.cpp`                      |
| **Semaphores**          | Resource counting, bounded buffer          | `03_producer_consumer_semaphore.cpp` |

---

## Common Patterns

### Protect Shared Data

```cpp
std::mutex mtx;
std::lock_guard<std::mutex> guard(mtx);
sharedData++;
```

### Wait for Signal

```cpp
std::condition_variable cv;
cv.wait(lock, []() { return ready; });
```

### Get Result from Thread

```cpp
auto future = std::async(compute, args);
int result = future.get();
```

### Lock Multiple Resources Safely

```cpp
std::scoped_lock lock(mtx1, mtx2);  // C++17
```

---

## Learning Tips

✅ **Read the documentation first** - Understand concepts before code  
✅ **Run each example** - See output, modify, experiment  
✅ **Try the exercises** - Create your own variations  
✅ **Progress sequentially** - Each topic builds on previous ones  
✅ **Keep code simple** - Focus on one concept per example

---

## Requirements

- **C++17** or later
- **GCC**, **Clang**, or **MSVC** compiler
- **pthread** library (usually included)

---

## Resources

- [C++ Reference: Threading](https://en.cppreference.com/w/cpp/thread)
- [C++ Reference: Mutex](https://en.cppreference.com/w/cpp/thread/mutex)
- [C++ Reference: Condition Variables](https://en.cppreference.com/w/cpp/thread/condition_variable)
