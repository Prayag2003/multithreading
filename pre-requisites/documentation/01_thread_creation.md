# Thread Creation Methods 🚀

## Overview

There are multiple ways to create and start threads in C++. Each method has its own use case. Let's explore them!

## Method 1: Function Pointer

Create a thread using a regular function:

```cpp
void printMessage(const std::string& msg) {
    std::cout << "Message: " << msg << "\n";
}

int main() {
    std::thread t(printMessage, "Hello, Thread!");
    t.join();
    return 0;
}
```

**When to use**: Simple functions, straightforward logic

## Method 2: Function Objects (Functors)

Use a class with `operator()`:

```cpp
class Worker {
public:
    void operator()() {
        std::cout << "Working...\n";
    }
};

int main() {
    Worker worker;
    std::thread t(worker);
    t.join();
    return 0;
}
```

**When to use**: When you need to maintain state

## Method 3: Lambda Functions

Anonymous functions (modern C++ preferred):

```cpp
int main() {
    int data = 42;
    std::thread t([data]() {
        std::cout << "Data: " << data << "\n";
    });
    t.join();
    return 0;
}
```

**When to use**: Quick, inline operations (most common in modern code)

## Passing Arguments to Threads

### By Value (Copy)

```cpp
int count = 10;
std::thread t([count]() {
    std::cout << count << "\n";  // count is copied
});
t.join();
```

**Pros**: Thread is independent  
**Cons**: Uses extra memory

### By Reference with `std::ref`

```cpp
int count = 10;
std::thread t([&count]() {
    count++;  // Modifies the original
});
t.join();
```

**Pros**: Efficient, can modify original  
**Cons**: Dangerous if original is destroyed!

## Important: Lifetime and References ⚠️

```cpp
// ❌ DANGEROUS!
void dangerousExample() {
    int localVar = 5;
    std::thread t([&localVar]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << localVar << "\n";  // localVar might be destroyed!
    });
    t.detach();  // Thread runs after function returns
    // localVar destroyed here, but thread still running!
}

// ✅ SAFE!
void safeExample() {
    int localVar = 5;
    std::thread t([localVar]() {  // Copy, not reference
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << localVar << "\n";  // Safe, has its own copy
    });
    t.detach();
}
```

## Passing Multiple Arguments

```cpp
void add(int a, int b) {
    std::cout << "Sum: " << (a + b) << "\n";
}

int main() {
    std::thread t(add, 5, 3);  // Arguments after function
    t.join();
    return 0;
}
```

## Thread IDs and Management

```cpp
#include <thread>
#include <iostream>

int main() {
    std::thread t1([]() { std::cout << "Thread 1\n"; });
    std::thread t2([]() { std::cout << "Thread 2\n"; });

    std::cout << "Main thread ID: " << std::this_thread::get_id() << "\n";
    std::cout << "t1 ID: " << t1.get_id() << "\n";

    t1.join();
    t2.join();

    return 0;
}
```

## Best Practices

✅ **Prefer lambdas** for new code (cleaner, more readable)  
✅ **Use std::ref** when you intentionally need references  
✅ **Copy small data** (ints, strings) by value  
✅ **Be aware of lifetimes** — ensure referenced objects survive  
✅ **Keep thread work focused** — one task per thread

## Quick Comparison Table

| Method   | Syntax                | Use Case          | State    |
| -------- | --------------------- | ----------------- | -------- |
| Function | `std::thread(func)`   | Simple operations | No       |
| Functor  | `std::thread(object)` | Complex logic     | Yes      |
| Lambda   | `std::thread([](){})` | Quick tasks       | Optional |
