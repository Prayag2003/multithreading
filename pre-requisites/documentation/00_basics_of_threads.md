# Basics of Threads 🧵

## What are Threads?

A thread is the smallest unit of a program that can run independently. Think of it like having multiple workers in a kitchen — each worker (thread) can perform tasks simultaneously instead of one person doing everything sequentially.

## Why Use Threads?

- **Concurrency**: Do multiple things at the same time
- **Responsiveness**: Keep your program responsive while performing long-running tasks
- **Performance**: Utilize multiple CPU cores efficiently

## Key Concepts

### Creating a Thread

In C++, you use `std::thread` to create a new thread:

```cpp
#include <thread>

void myFunction() {
    std::cout << "Running in a thread!\n";
}

int main() {
    std::thread t(myFunction);  // Create and start a thread
    t.join();                    // Wait for thread to finish
    return 0;
}
```

### Understanding Thread Execution

When you create a thread, the program execution **splits**:

- The main thread continues executing the next line
- The new thread runs your function concurrently

### Race Conditions ⚠️

**What is a race condition?**

A race condition occurs when multiple threads access shared data without synchronization. The result depends on the timing of thread execution — unpredictable!

**Example Problem:**

```cpp
int counter = 0;

void incrementCounter() {
    for (int i = 0; i < 1000; i++) {
        counter++;  // NOT thread-safe!
    }
}

// If you run this with multiple threads, counter might not be 2000!
```

## Common Patterns

### Serial Execution (No Threads)

```
Task 1 → Task 2 → Task 3
Time: 3 seconds
```

### Parallel Execution (With Threads)

```
Task 1 ─┐
Task 2 ─├─> Runs simultaneously
Task 3 ─┘
Time: ~1 second
```

## Next Steps

1. **Understand thread creation**: See `01_thread_creation.cpp`
2. **Learn about thread lifecycle**: See `02_join_detach.cpp`
3. **Protect shared data**: See `03_mutex.cpp`

## Common Mistakes

- ❌ **Don't**: Let threads access shared variables without protection
- ❌ **Don't**: Create more threads than your CPU has cores (usually)
- ❌ **Don't**: Forget to `join()` threads before program exits

- ✅ **Do**: Use mutexes to protect shared data
- ✅ **Do**: Keep thread work simple and focused
- ✅ **Do**: Always clean up threads with `join()` or `detach()`

## Quick Reference

```cpp
#include <thread>

std::thread t(function);   // Create thread
t.join();                  // Wait for completion
t.joinable();              // Check if can be joined
t.get_id();                // Get thread ID
```
