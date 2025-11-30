# Thread Lifetime: join() vs detach() 🔄

## Thread Ownership

When you create a `std::thread`, you own it and are responsible for cleaning it up. There are two ways to clean up:

## join() - Wait for Thread to Finish

`join()` makes the main thread **wait** until the spawned thread completes.

```cpp
#include <thread>
#include <iostream>

int main() {
    std::thread t([]() {
        std::cout << "Doing work...\n";
    });

    std::cout << "Main thread continues\n";
    t.join();  // Wait for thread to finish
    std::cout << "Thread finished, main continues\n";

    return 0;
}
```

**Output:**

```
Main thread continues
Doing work...
Thread finished, main continues
```

## detach() - Let Thread Run Independently

`detach()` lets the thread run on its own — the main thread doesn't wait.

```cpp
#include <thread>
#include <iostream>

int main() {
    std::thread t([]() {
        std::cout << "Doing work...\n";
    });

    std::cout << "Main thread continues\n";
    t.detach();  // Let thread run independently
    std::cout << "Main thread exits\n";

    return 0;  // Main thread ends, program exits
}
```

**Possible Output:**

```
Main thread continues
Main thread exits
```

**Note**: The "Doing work..." might not print because the program exits before the thread finishes!

## Comparison Table

| Feature                 | `join()`        | `detach()` |
| ----------------------- | --------------- | ---------- |
| **Wait for completion** | ✅ Yes          | ❌ No      |
| **Predictable cleanup** | ✅ Yes          | ⚠️ No      |
| **Resource access**     | ✅ Safe         | ❌ Risky   |
| **Program termination** | ✅ Synchronized | ⚠️ Abrupt  |
| **Difficulty**          | Easy            | Advanced   |

## When to Use join()

✅ **Most of the time!** Use `join()` when:

- You need results from the thread
- You want predictable behavior
- The work must complete before continuing
- You're learning threading basics

```cpp
// Typical pattern
std::thread t(doWork);
// ... optionally do other work ...
t.join();  // Wait before proceeding
```

## When to Use detach()

⚠️ **Rarely, and carefully!** Use `detach()` only when:

- The thread has unlimited lifetime (daemon threads)
- The thread doesn't access program resources
- You have a manager thread pool

```cpp
// Example: Background logging thread
std::thread loggerThread([]() {
    while (true) {
        // Log messages to file
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
});
loggerThread.detach();  // Let it run forever
```

## Common Pitfalls ⚠️

### Mistake 1: Forgetting join() or detach()

```cpp
// ❌ WRONG - Thread is not cleaned up!
void badExample() {
    std::thread t([]() { std::cout << "Work\n"; });
    // No join() or detach()!
}  // Program crashes or hangs here

// ✅ CORRECT
void goodExample() {
    std::thread t([]() { std::cout << "Work\n"; });
    t.join();
}
```

### Mistake 2: Accessing Destroyed Data After detach()

```cpp
// ❌ DANGEROUS
void danglingReference() {
    int value = 42;
    std::thread t([&value]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << value << "\n";  // value might be destroyed!
    });
    t.detach();
}  // value destroyed, thread still running

// ✅ SAFE - Copy the value
void safeVersion() {
    int value = 42;
    std::thread t([value]() {  // Copy, not reference
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << value << "\n";  // Safe!
    });
    t.detach();
}
```

## Visual Comparison

### join() Timeline

```
Main Thread  ─────┬─────────────────────────────►
                  │
Worker Thread ────┴─► [Do Work] ──►
                      ↑
                   join() waits here
```

### detach() Timeline

```
Main Thread  ─────┬─► [Continue] ──► [Exit]
                  │
Worker Thread ────┴─► [Do Work] ──► [End] (might continue after main exits!)
```

## Checking Thread State

```cpp
std::thread t([]() { std::cout << "Work\n"; });

if (t.joinable()) {
    std::cout << "Thread is still running\n";
    t.join();
} else {
    std::cout << "Thread already finished or detached\n";
}
```

## Best Practices

✅ **Default to join()** for safety and predictability  
✅ **Always check joinable()** before calling join()  
✅ **Copy data** when using detach() to avoid dangling references  
✅ **Document why** if you use detach()  
✅ **Use RAII wrappers** for automatic cleanup

## RAII Pattern (Advanced)

```cpp
class ScopedThread {
    std::thread t;
public:
    template<typename F>
    ScopedThread(F&& f) : t(std::forward<F>(f)) {}

    ~ScopedThread() {
        if (t.joinable()) t.join();  // Automatic cleanup!
    }
};

int main() {
    {
        ScopedThread t([]() { std::cout << "Work\n"; });
    }  // Automatically joined here
    return 0;
}
```
