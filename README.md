# C++ Threading & Synchronization Learning Guide 🧵

A beginner-friendly learning repository for C++ concurrency concepts. Learn step-by-step with clear examples and explanations.

## 🔗 Resources

- [🔴 YouTube: Cpp Nuts Multithreading Playlist](https://youtu.be/TPVH_coGAQs)
- [C++ Reference: Threading](https://en.cppreference.com/w/cpp/thread)
- [C++ Reference: Mutex](https://en.cppreference.com/w/cpp/thread/mutex)
- [C++ Reference: Condition Variables](https://en.cppreference.com/w/cpp/thread/condition_variable)

## 📚 What You'll Learn

- **Basics**: Core threading concepts from scratch
- **Implementations**: Real-world patterns and solutions
- **Best Practices**: How to write safe, efficient concurrent code

## 🚀 Quick Start

### Build All Examples

```bash
# Create output directory
mkdir -p bin

# Build basics examples
for f in fundamentals/basics/*.cpp; do
    name=$(basename "$f" .cpp)
    g++ -std=c++20 -Wall -Wextra -pthread "$f" -o "bin/$name"
done

# Build implementation examples
for f in fundamentals/implementations/*.cpp; do
    name=$(basename "$f" .cpp)
    g++ -std=c++20 -Wall -Wextra -pthread "$f" -o "bin/$name"
done
```

### Run an Example

```bash
./bin/00_basics_of_threads
```

---

## 📖 Basics: Threading Fundamentals

Start here! Learn core concepts step by step. Each example builds on the previous one.

| #   | Code File                                                                        | Documentation                                                                          | What You'll Learn                                          |
| --- | -------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------- | ---------------------------------------------------------- |
| 00  | [`00_basics_of_threads.cpp`](fundamentals/basics/00_basics_of_threads.cpp)       | [📖 Basics of Threads](fundamentals/basics/documentation/00_basics_of_threads.md)      | What threads are, why they're useful, race conditions      |
| 01  | [`01_thread_creation.cpp`](fundamentals/basics/01_thread_creation.cpp)           | [📖 Thread Creation](fundamentals/basics/documentation/01_thread_creation.md)          | Functions, functors, lambdas, passing arguments            |
| 02  | [`02_join_detach.cpp`](fundamentals/basics/02_join_detach.cpp)                   | [📖 join() vs detach()](fundamentals/basics/documentation/02_join_detach.md)           | Thread lifetime, waiting, independent threads              |
| 03  | [`03_mutex.cpp`](fundamentals/basics/03_mutex.cpp)                               | [📖 Mutex Protection](fundamentals/basics/documentation/03_mutex.md)                   | Protecting shared data, critical sections, locks           |
| 04  | [`04_try_lock.cpp`](fundamentals/basics/04_try_lock.cpp)                         | [📖 try_lock()](fundamentals/basics/documentation/04_try_lock.md)                      | Non-blocking lock attempts, avoiding deadlocks             |
| 05  | [`05_multiple_try_lock.cpp`](fundamentals/basics/05_multiple_try_lock.cpp)       | [📖 Multiple Locks](fundamentals/basics/documentation/05_multiple_try_lock.md)         | Locking multiple mutexes safely, `std::lock()`             |
| 06  | [`06_recursion_lock.cpp`](fundamentals/basics/06_recursion_lock.cpp)             | [📖 Recursive Mutex](fundamentals/basics/documentation/06_recursion_lock.md)           | Recursive locking, reentrancy, `std::recursive_mutex`      |
| 07  | [`07_lock_guard.cpp`](fundamentals/basics/07_lock_guard.cpp)                     | [📖 Lock Guard](fundamentals/basics/documentation/07_lock_guard.md)                    | RAII pattern, automatic unlocking, exception safety        |
| 08  | [`08_unique_lock.cpp`](fundamentals/basics/08_unique_lock.cpp)                   | [📖 Unique Lock](fundamentals/basics/documentation/08_unique_lock.md)                  | Manual control, deferred locking, timed waits              |
| 09  | [`09_conditional_variable.cpp`](fundamentals/basics/09_conditional_variable.cpp) | [📖 Condition Variables](fundamentals/basics/documentation/09_conditional_variable.md) | Thread signaling, producer-consumer, `wait()` / `notify()` |
| 10  | [`10_deadlock.cpp`](fundamentals/basics/10_deadlock.cpp)                         | [📖 Deadlock Prevention](fundamentals/basics/documentation/10_deadlock.md)             | Circular waits, safe lock ordering, detection              |
| 11  | [`11_future_and_promise.cpp`](fundamentals/basics/11_future_and_promise.cpp)     | [📖 Futures & Promises](fundamentals/basics/documentation/11_future_and_promise.md)    | Getting return values from threads, exception passing      |
| 12  | [`12_async.cpp`](fundamentals/basics/12_async.cpp)                               | [📖 std::async](fundamentals/basics/documentation/12_async.md)                         | Simplest concurrency, automatic thread management          |

---

## 🏗️ Implementations: Real-World Patterns

Apply what you've learned to solve practical problems.

| #   | Code File                                                                                               | Documentation                                                                                                    | What You'll Learn                                |
| --- | ------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------- | ------------------------------------------------ |
| 00  | [`00_producer_consumer_mutex.cpp`](fundamentals/implementations/00_producer_consumer_mutex.cpp)         | [📖 Producer-Consumer (Mutex)](fundamentals/implementations/documentation/00_producer_consumer_mutex.md)         | Bounded buffer pattern, mutex-based coordination |
| 01  | [`01_atomic.cpp`](fundamentals/implementations/01_atomic.cpp)                                           | [📖 Atomics](fundamentals/implementations/documentation/01_atomic.md)                                            | Lock-free synchronization, atomic operations     |
| 02  | [`02_binary_semaphore.cpp`](fundamentals/implementations/02_binary_semaphore.cpp)                       | [📖 Binary Semaphore](fundamentals/implementations/documentation/02_binary_semaphore.md)                         | Semaphore basics, acquire/release pattern        |
| 03  | [`03_producer_consumer_semaphore.cpp`](fundamentals/implementations/03_producer_consumer_semaphore.cpp) | [📖 Producer-Consumer (Semaphore)](fundamentals/implementations/documentation/03_producer_consumer_semaphore.md) | Elegant bounded buffer with semaphores           |

---

## 🎯 Key Concepts Quick Reference

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

## 💡 Common Patterns

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

## 📝 Learning Tips

✅ **Read the documentation first** - Understand concepts before diving into code  
✅ **Run each example** - See the output, modify it, experiment  
✅ **Progress sequentially** - Each topic builds on previous ones  
✅ **Keep it simple** - Focus on one concept per example  
✅ **Practice** - Try creating your own variations

---

## 🔧 Requirements

- **C++20** or later
- **GCC**, **Clang**, or **MSVC** compiler
- **pthread** library (usually included with compiler)

---

## 📚 Project Structure

```
threadpool/
├── fundamentals/
│   ├── basics/
│   │   ├── *.cpp                    # Example code files
│   │   └── documentation/          # Documentation for basics
│   │       └── *.md
│   └── implementations/
│       ├── *.cpp                    # Implementation examples
│       └── documentation/           # Documentation for implementations
│           └── *.md
├── threadpool.cpp                   # Main threadpool implementation
└── README.md                        # This file
```

---

## 🎓 How to Use This Repository

1. **Start with Basics**: Begin with `00_basics_of_threads.cpp` and work through sequentially
2. **Read Documentation**: Each code file has corresponding documentation explaining the concepts
3. **Run Examples**: Compile and run each example to see it in action
4. **Experiment**: Modify the code to understand how changes affect behavior
5. **Move to Implementations**: Once comfortable with basics, explore real-world patterns

---

**Happy Learning! 🚀**
