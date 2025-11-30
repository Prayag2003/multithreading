# join() vs detach() 🔗

## Overview

When you create a thread in C++, you must decide how to manage its lifetime. The two main options are `join()` and `detach()`. Understanding the difference is crucial for writing correct concurrent programs.

## Thread Lifecycle

```
Created → Running → [join/detach] → Terminated
```

After a thread is created, it must be either:
- **Joined**: Main thread waits for it to complete
- **Detached**: Thread runs independently

## join() - Wait for Completion

### Definition

`join()` blocks the calling thread until the target thread completes execution. The calling thread waits for the worker thread to finish.

### Syntax

```cpp
std::thread t(function, args);
t.join();  // Blocks until thread completes
```

### Example

```cpp
#include <thread>
#include <chrono>
#include <iostream>

void Join(int count) {
    while (count-- > 0) {
        std::cout << "Hello\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Thread finished\n";
}

int main() {
    std::thread t(Join, 5);
    std::cout << "Main thread continues...\n";
    
    t.join();  // Main thread waits here
    
    std::cout << "Main thread continues after join\n";
    return 0;
}
```

### Output

```
Main thread continues...
Hello
Hello
Hello
Hello
Hello
Thread finished
Main thread continues after join
```

### Key Points

1. **Blocking call**: `join()` blocks until thread completes
2. **Synchronization**: Ensures thread finishes before continuing
3. **Resource cleanup**: Thread resources are cleaned up after join
4. **Double join**: Calling `join()` twice causes program termination

### Checking if Joinable

```cpp
std::thread t(work);

if (t.joinable()) {
    t.join();  // Safe to join
} else {
    // Thread already joined or detached
}
```

### When to Use join()

✅ **Use when:**
- You need the result from the thread
- You need to ensure thread completes before continuing
- You need to clean up resources
- Thread produces data needed by main thread

### Example: Getting Results

```cpp
int result = 0;

void compute(int& output) {
    output = 42 * 2;  // Some computation
}

int main() {
    std::thread t(compute, std::ref(result));
    t.join();  // Must wait to get result
    
    std::cout << "Result: " << result << "\n";  // Safe to use
    return 0;
}
```

## detach() - Independent Execution

### Definition

`detach()` separates the thread from the `std::thread` object, allowing it to run independently. The thread becomes a "daemon" thread that runs in the background.

### Syntax

```cpp
std::thread t(function, args);
t.detach();  // Thread runs independently
```

### Example

```cpp
void Join(int count) {
    while (count-- > 0) {
        std::cout << "Hello\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Thread finished\n";
}

int main() {
    std::thread t(Join, 5);
    std::cout << "Main thread continues...\n";
    
    t.detach();  // Thread runs independently
    
    std::cout << "Main thread continues immediately\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;  // Program may exit before thread finishes!
}
```

### Output

```
Main thread continues...
Main thread continues immediately
Hello
Hello
...
```

### Key Points

1. **Non-blocking**: `detach()` returns immediately
2. **Independent**: Thread runs independently from main thread
3. **No control**: Cannot join or interact with thread after detach
4. **Double detach**: Calling `detach()` twice causes program termination
5. **Lifetime**: Thread must complete before program exits (or use `std::atexit`)

### When to Use detach()

✅ **Use when:**
- Thread is a background task (daemon)
- You don't need the result
- Thread manages its own lifetime
- Fire-and-forget operations

### Example: Background Logger

```cpp
void logMessage(const std::string& msg) {
    // Write to log file
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Logged: " << msg << "\n";
}

int main() {
    std::thread logger(logMessage, "Application started");
    logger.detach();  // Run in background
    
    // Main program continues
    std::cout << "Main program running...\n";
    return 0;
}
```

## Comparison

| Feature | join() | detach() |
|---------|--------|----------|
| **Blocking** | Yes, waits | No, returns immediately |
| **Control** | Can wait for result | No control after detach |
| **Synchronization** | Synchronizes threads | Independent execution |
| **Resource cleanup** | Automatic after join | Automatic on completion |
| **Use case** | Need results | Background tasks |
| **Safety** | Safe, controlled | Must ensure thread completes |

## Important Rules

### Rule 1: Always Call join() or detach()

```cpp
// ❌ WRONG - Program terminates
std::thread t(work);
// Thread destroyed without join/detach

// ✅ CORRECT
std::thread t(work);
t.join();  // or t.detach()
```

### Rule 2: Cannot Join/Detach Twice

```cpp
std::thread t(work);
t.join();
t.join();  // ❌ Program termination!

std::thread t2(work);
t2.detach();
t2.detach();  // ❌ Program termination!
```

### Rule 3: Check Before Joining

```cpp
std::thread t(work);
t.detach();

if (t.joinable()) {  // Returns false after detach
    t.join();  // Won't execute
}
```

### Rule 4: Thread Object Can Be Destroyed After Detach

```cpp
{
    std::thread t(work);
    t.detach();
}  // Thread object destroyed, but thread still running
// Thread continues independently
```

## Complete Example

```cpp
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>

void worker(int id, int duration) {
    std::cout << "Worker " << id << " started\n";
    std::this_thread::sleep_for(std::chrono::seconds(duration));
    std::cout << "Worker " << id << " finished\n";
}

int main() {
    std::vector<std::thread> threads;
    
    // Create threads
    for (int i = 0; i < 3; i++) {
        threads.push_back(std::thread(worker, i, 2));
    }
    
    // Join all threads
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    std::cout << "All threads completed\n";
    return 0;
}
```

## RAII Wrapper Example

```cpp
class ThreadGuard {
    std::thread& t;
public:
    explicit ThreadGuard(std::thread& t_) : t(t_) {}
    
    ~ThreadGuard() {
        if (t.joinable()) {
            t.join();
        }
    }
    
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
};

void work() {
    // Do work
}

int main() {
    std::thread t(work);
    ThreadGuard g(t);  // Automatically joins in destructor
    return 0;
}
```

## Common Mistakes

### Mistake 1: Forgetting to Join

```cpp
// ❌ Wrong
void createThread() {
    std::thread t(work);
    // Forgot join/detach - program may crash
}

// ✅ Correct
void createThread() {
    std::thread t(work);
    t.join();  // or t.detach()
}
```

### Mistake 2: Detaching with Local Variables

```cpp
// ❌ Dangerous
void dangerous() {
    int local = 10;
    std::thread t([&local]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << local;  // local destroyed!
    });
    t.detach();
    // local destroyed here
}

// ✅ Safe
void safe() {
    int local = 10;
    std::thread t([local]() {  // Copy
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << local;  // Safe copy
    });
    t.detach();
}
```

### Mistake 3: Joining After Detach

```cpp
// ❌ Wrong
std::thread t(work);
t.detach();
t.join();  // Error: thread not joinable

// ✅ Correct
std::thread t(work);
t.join();  // or t.detach(), but not both
```

## Best Practices

✅ **Always join or detach** before thread object is destroyed  
✅ **Prefer join()** when you need results or synchronization  
✅ **Use detach()** only for true background tasks  
✅ **Check joinable()** before joining if unsure  
✅ **Use RAII wrappers** for automatic management

## Next Steps

- Learn to protect shared data: `03_mutex.cpp`
