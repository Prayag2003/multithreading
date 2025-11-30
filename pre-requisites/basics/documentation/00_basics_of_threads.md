# Basics of Threads 🧵

## What are Threads?

A **thread** is a lightweight process used to achieve parallelism by dividing a process into multiple threads. Each thread can execute independently and share the same memory space with other threads in the same process.

### Standard Definition

A thread is the smallest unit of execution that can be managed independently by a scheduler. Threads within the same process share:
- Code section
- Data section (global variables, heap)
- Open files and other resources

But each thread has its own:
- Stack (local variables)
- Program counter
- Register set

### Real-World Examples

- **Web Browser**: Multiple tabs run in separate threads
- **Text Editor**: One thread for typing, another for auto-save, another for spell-check
- **Video Player**: One thread for decoding video, another for audio, another for UI updates
- **Web Server**: Each client request handled by a separate thread

**Important:** Thread execution order is not defined - it can be any order! The operating system scheduler decides when each thread runs.

## Why Use Threads?

### Benefits

1. **Parallelism**: Execute multiple tasks simultaneously
2. **Performance**: Utilize multiple CPU cores efficiently
3. **Responsiveness**: Keep UI responsive while doing background work
4. **Resource Sharing**: Threads share memory, making data sharing easier

### When to Use Threads

- CPU-intensive tasks that can be parallelized
- I/O operations (file reading, network requests)
- Background processing while maintaining UI responsiveness
- Tasks that can be divided into independent parts

## Understanding the Code

The example code demonstrates parallel computation:

```cpp
ull oddSum = 0, evenSum = 0;

void findEven(ull start, ull end) {
    for(ull i = start; i <= end; i++) {
        if(!(i & 1)) {  // Check if even
            evenSum += i;
        }
    }
}

void findOdd(ull start, ull end) {
    for(ull i = start; i <= end; i++) {
        if(i & 1) {  // Check if odd
            oddSum += i;
        }
    }
}

int main() {
    ull start = 0, end = 1900000000;
    auto startTime = high_resolution_clock::now();

    // Create two threads
    std::thread t1(findOdd, start, end);
    std::thread t2(findEven, start, end);

    // Wait for both threads to complete
    t1.join();
    t2.join();

    auto stopTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stopTime - startTime);

    cout << "Odd Sum: " << oddSum << "\n";
    cout << "Even Sum: " << evenSum << "\n";
    cout << "Time taken: " << duration.count() << " seconds\n";
}
```

### What the Code Does

1. **Creates two threads**: One calculates odd sum, another calculates even sum
2. **Parallel execution**: Both threads run simultaneously on different CPU cores
3. **Synchronization**: `join()` ensures main thread waits for completion
4. **Performance measurement**: Compares time with sequential execution

### Performance Comparison

**Sequential (without threads):**
```
Time: ~10 seconds (example)
```

**Parallel (with threads):**
```
Time: ~5 seconds (example) - Nearly 2x faster!
```

## Key Concepts

### Creating a Thread

```cpp
#include <thread>

std::thread t1(functionName, arg1, arg2);
```

- Thread starts executing immediately
- Arguments are passed by value (use `std::ref` for references)

### Waiting for Thread to Finish

```cpp
t1.join();  // Blocks until thread completes
```

- `join()` makes the calling thread wait
- Must call `join()` or `detach()` before thread object is destroyed

### Thread States

1. **Created**: Thread object created, thread starts
2. **Running**: Thread executing its function
3. **Blocked**: Thread waiting (e.g., for I/O, mutex)
4. **Terminated**: Thread function completed

## Race Conditions ⚠️

### Definition

A **race condition** occurs when multiple threads access shared data without synchronization, and the final result depends on the timing of thread execution.

### Example Problem

```cpp
int counter = 0;

void increment() {
    for (int i = 0; i < 1000; i++) {
        counter++;  // NOT thread-safe!
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    
    // Expected: 2000
    // Actual: Random value (1500, 1800, etc.)
    cout << counter << "\n";
}
```

### Why It Happens

The operation `counter++` is actually three steps:
1. Read current value
2. Increment it
3. Write it back

If two threads do this simultaneously:
```
Thread 1: Read (0) → Increment → Write (1)
Thread 2: Read (0) → Increment → Write (1)  // Lost update!
```

### Solution

Use synchronization primitives (mutex, atomic) to protect shared data. See `03_mutex.cpp`.

## Thread vs Process

| Feature | Thread | Process |
|---------|--------|---------|
| Memory | Shared | Isolated |
| Creation | Faster | Slower |
| Communication | Shared memory | IPC required |
| Overhead | Low | High |
| Crash impact | Affects process | Isolated |

## Best Practices

✅ **Do:**
- Use threads for independent tasks
- Always call `join()` or `detach()`
- Protect shared data with mutexes
- Keep thread work focused and simple

❌ **Don't:**
- Create too many threads (usually 1-2x CPU cores)
- Access shared data without protection
- Forget to join/detach threads
- Assume execution order

## Next Steps

- Learn different ways to create threads: `01_thread_creation.cpp`
- Understand thread lifecycle: `02_join_detach.cpp`
- Learn to protect shared data: `03_mutex.cpp`
