# C++ Threading & Synchronization Learning Guide 🧵

A beginner-friendly learning repository for C++ concurrency concepts. Learn step-by-step with clear examples and explanations.

## 🔗 Resources

- [🔴 YouTube: Cpp Nuts Multithreading Playlist](https://youtu.be/TPVH_coGAQs)
- [C++ Reference: Threading](https://en.cppreference.com/w/cpp/thread)
- [C++ Reference: Mutex](https://en.cppreference.com/w/cpp/thread/mutex)
- [C++ Reference: Condition Variables](https://en.cppreference.com/w/cpp/thread/condition_variable)

## 📚 What You'll Learn

- **Fundamentals**: Core threading concepts from scratch
- **Implementations**: Real-world patterns and solutions
- **Best Practices**: How to write safe, efficient concurrent code

---

## 🚀 Quick Start

### Build All Examples (using Makefile)

```bash
git clone https://github.com/Prayag2003/multithreading
cd multithreading
make
```

### Clean all built binaries

```bash
make clean
```

### Run an Example

```bash
./bin/00_basics_of_threads
```

---

## 📖 Fundamentals: Threading Basics

Start here! Each example builds on the previous one.

| #   | Code File                                                                 | Documentation                                                                   | What You'll Learn                           |
| --- | ------------------------------------------------------------------------- | ------------------------------------------------------------------------------- | ------------------------------------------- |
| 00  | [`00_basics_of_threads.cpp`](fundamentals/00_basics_of_threads.cpp)       | [📖 Basics of Threads](fundamentals/documentation/00_basics_of_threads.md)      | What threads are, race conditions           |
| 01  | [`01_thread_creation.cpp`](fundamentals/01_thread_creation.cpp)           | [📖 Thread Creation](fundamentals/documentation/01_thread_creation.md)          | Functions, functors, lambdas                |
| 02  | [`02_join_detach.cpp`](fundamentals/02_join_detach.cpp)                   | [📖 join() vs detach()](fundamentals/documentation/02_join_detach.md)           | Thread lifetime, waiting                    |
| 03  | [`03_mutex.cpp`](fundamentals/03_mutex.cpp)                               | [📖 Mutex Protection](fundamentals/documentation/03_mutex.md)                   | Protecting shared data                      |
| 04  | [`04_try_lock.cpp`](fundamentals/04_try_lock.cpp)                         | [📖 try_lock()](fundamentals/documentation/04_try_lock.md)                      | Non-blocking locks                          |
| 05  | [`05_multiple_try_lock.cpp`](fundamentals/05_multiple_try_lock.cpp)       | [📖 Multiple Locks](fundamentals/documentation/05_multiple_try_lock.md)         | Locking multiple mutexes safely             |
| 06  | [`06_recursion_lock.cpp`](fundamentals/06_recursion_lock.cpp)             | [📖 Recursive Mutex](fundamentals/documentation/06_recursion_lock.md)           | Recursive locking                           |
| 07  | [`07_lock_guard.cpp`](fundamentals/07_lock_guard.cpp)                     | [📖 Lock Guard](fundamentals/documentation/07_lock_guard.md)                    | RAII, exception safety                      |
| 08  | [`08_unique_lock.cpp`](fundamentals/08_unique_lock.cpp)                   | [📖 Unique Lock](fundamentals/documentation/08_unique_lock.md)                  | Deferred locking, timed waits               |
| 09  | [`09_conditional_variable.cpp`](fundamentals/09_conditional_variable.cpp) | [📖 Condition Variables](fundamentals/documentation/09_conditional_variable.md) | Thread signaling, producer-consumer pattern |
| 10  | [`10_deadlock.cpp`](fundamentals/10_deadlock.cpp)                         | [📖 Deadlock Prevention](fundamentals/documentation/10_deadlock.md)             | Circular waits, prevention                  |
| 11  | [`11_future_and_promise.cpp`](fundamentals/11_future_and_promise.cpp)     | [📖 Futures & Promises](fundamentals/documentation/11_future_and_promise.md)    | Getting results from threads                |
| 12  | [`12_async.cpp`](fundamentals/12_async.cpp)                               | [📖 std::async](fundamentals/documentation/12_async.md)                         | Automatic async execution                   |

---

## 🏗️ Implementations: Real-World Patterns

Your applied learning section.

| #   | Code File                                                                                  | Documentation                                                                                       | What You'll Learn                           |
| --- | ------------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------- | ------------------------------------------- |
| 00  | [`00_producer_consumer_mutex.cpp`](implementations/00_producer_consumer_mutex.cpp)         | [📖 Producer-Consumer (Mutex)](implementations/documentation/00_producer_consumer_mutex.md)         | Bounded buffer, mutex-based synchronization |
| 01  | [`01_atomic.cpp`](implementations/01_atomic.cpp)                                           | [📖 Atomics](implementations/documentation/01_atomic.md)                                            | Lock-free counters, atomic operations       |
| 02  | [`02_binary_semaphore.cpp`](implementations/02_binary_semaphore.cpp)                       | [📖 Binary Semaphore](implementations/documentation/02_binary_semaphore.md)                         | Semaphore basics, acquire/release pattern   |
| 03  | [`03_producer_consumer_semaphore.cpp`](implementations/03_producer_consumer_semaphore.cpp) | [📖 Producer-Consumer (Semaphore)](implementations/documentation/03_producer_consumer_semaphore.md) | Elegant bounded buffer using semaphores     |

---

## 🎯 Key Concepts Quick Reference

| Concept                 | Use When                        | See Example                          |
| ----------------------- | ------------------------------- | ------------------------------------ |
| **Mutex**               | Protecting shared data          | `03_mutex.cpp`                       |
| **Lock Guard**          | Simple RAII locking             | `07_lock_guard.cpp`                  |
| **Unique Lock**         | Manual lock control             | `08_unique_lock.cpp`                 |
| **Condition Variables** | Thread coordination             | `09_conditional_variable.cpp`        |
| **std::async**          | Fetching results from threads   | `12_async.cpp`                       |
| **Atomics**             | Lock-free shared counters/flags | `01_atomic.cpp`                      |
| **Semaphores**          | Resource counting               | `03_producer_consumer_semaphore.cpp` |

---

## 💡 Common Patterns

### Protect Shared Data

```cpp
std::mutex mtx;
std::lock_guard<std::mutex> guard(mtx);
```

### Wait for a Signal

```cpp
cv.wait(lock, [] { return ready; });
```

### Get a Result from a Thread

```cpp
auto future = std::async(compute);
int result = future.get();
```

### Lock Multiple Resources Safely

```cpp
std::scoped_lock lock(m1, m2);
```

---

## 🔧 Requirements

- **C++20** or later
- GCC / Clang / MSVC
- pthread support

---

## 📚 Project Structure (UPDATED)

```
threading-guide/
├── fundamentals/
│   ├── *.cpp                    # Basic threading examples
│   └── documentation/           # Markdown docs for fundamentals
│       └── *.md
├── implementations/
│   ├── *.cpp                    # Advanced real-world patterns
│   └── documentation/           # Markdown docs for implementations
│       └── *.md
├── bin/                         # Auto-created binaries (via Makefile)
├── Makefile
└── README.md
```

---

## 🎓 How to Use This Repository

1. **Start with Fundamentals** → Work through 00 → 12
2. **Read Documentation** → Understand each concept deeply
3. **Run Examples** → See how they work
4. **Experiment** → Modify values, add threads, cause bugs
5. **Move to Implementations** → Learn real-world concurrency patterns
