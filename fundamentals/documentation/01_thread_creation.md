# Thread Creation Methods 🚀

## Overview

C++ provides multiple ways to create threads. Each method has its own use case and syntax. Understanding all methods helps you choose the right one for your situation.

## Standard Thread Creation

In C++, threads are created using `std::thread` from `<thread>` header:

```cpp
#include <thread>
std::thread thread_object(callable, args...);
```

The thread starts executing immediately when created.

## Method 1: Function Pointer

Pass a regular function to the thread.

### Example

```cpp
void fun(int x) {
    while (x-- > 0) {
        std::cout << x << "\n";
    }
}

void usingFun() {
    std::thread t(fun, 10);  // Function pointer + argument
    t.join();
}
```

### When to Use

- Simple standalone functions
- Functions that don't need state
- Legacy code compatibility

### Passing Arguments

```cpp
void printSum(int a, int b) {
    std::cout << "Sum: " << (a + b) << "\n";
}

std::thread t(printSum, 5, 3);  // Pass multiple arguments
t.join();
```

## Method 2: Lambda Functions

Use anonymous lambda functions (most common in modern C++).

### Example

```cpp
void usingLambda() {
    std::thread t([](int x) {
        while(x-- > 0) {
            std::cout << x << "\n";
        }
    }, 10);
    t.join();
}
```

### Advantages

- **Concise**: Define function inline
- **Capture variables**: Access outer scope variables
- **Modern style**: Preferred in C++11+

### Capturing Variables

```cpp
int data = 42;
std::string message = "Hello";

// Capture by value
std::thread t1([data]() {
    std::cout << data << "\n";  // Copy of data
});

// Capture by reference
std::thread t2([&data]() {
    data++;  // Modifies original
});

// Capture all by value
std::thread t3([=]() {
    std::cout << data << message << "\n";
});

// Capture all by reference
std::thread t4([&]() {
    data++;
    message += " World";
});

// Mixed capture
std::thread t5([data, &message]() {
    std::cout << data << " " << message << "\n";
});
```

### Important: Lifetime Issues

```cpp
// ❌ DANGEROUS - Reference to local variable
void dangerous() {
    int localVar = 5;
    std::thread t([&localVar]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << localVar << "\n";  // localVar might be destroyed!
    });
    t.detach();
    // localVar destroyed here, but thread still running!
}

// ✅ SAFE - Capture by value
void safe() {
    int localVar = 5;
    std::thread t([localVar]() {  // Copy, not reference
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << localVar << "\n";  // Safe, has its own copy
    });
    t.join();  // Wait for completion
}
```

## Method 3: Functor (Function Object)

Use a class with overloaded `operator()`.

### Example

```cpp
class Base {
public:
    void operator()(int x) {
        while (x-- > 0) {
            std::cout << x << "\n";
        }
    }
};

void usingFunctor() {
    std::thread t(Base(), 10);  // Temporary object
    t.join();
}
```

### When to Use

- Need to maintain state
- Complex logic that benefits from encapsulation
- Reusable thread functions

### With State

```cpp
class Counter {
    int count;
public:
    Counter(int start) : count(start) {}

    void operator()() {
        for (int i = 0; i < 5; i++) {
            std::cout << "Count: " << count++ << "\n";
        }
    }
};

std::thread t(Counter(10));  // Starts from 10
t.join();
```

## Method 4: Non-Static Member Function

Call a member function of an object instance.

### Example

```cpp
class Base2 {
public:
    void trigger(int x) {
        while (x-- > 0) {
            std::cout << x << "\n";
        }
    }
};

void usingNonStaticMember() {
    Base2 b2;
    // Pass: pointer to member function, object pointer, arguments
    std::thread t(&Base2::trigger, &b2, 10);
    t.join();
}
```

### Syntax Explanation

```cpp
std::thread t(&ClassName::memberFunction, &object, arg1, arg2);
//            ^pointer to member        ^object pointer  ^arguments
```

### Why Object Pointer?

Non-static member functions need an object instance because they can access member variables. The object pointer provides the `this` context.

### Complete Example

```cpp
class Worker {
    int id;
public:
    Worker(int i) : id(i) {}

    void doWork(int iterations) {
        for (int i = 0; i < iterations; i++) {
            std::cout << "Worker " << id << " iteration " << i << "\n";
        }
    }
};

int main() {
    Worker w1(1), w2(2);

    std::thread t1(&Worker::doWork, &w1, 5);
    std::thread t2(&Worker::doWork, &w2, 5);

    t1.join();
    t2.join();
}
```

## Method 5: Static Member Function

Call a static member function (doesn't need object instance).

### Example

```cpp
class Base3 {
public:
    static void trigger(int x) {
        while (x-- > 0) {
            std::cout << x << "\n";
        }
    }
};

void usingStaticMember() {
    // No object needed for static functions
    std::thread t(&Base3::trigger, 10);
    t.join();
}
```

### When to Use

- Utility functions that don't need instance state
- Functions that operate on class-level data
- Similar to regular function pointers

## Passing Arguments: By Value vs By Reference

### By Value (Default)

```cpp
int count = 10;
std::thread t([count]() {  // count is copied
    std::cout << count << "\n";
});
t.join();
```

**Pros**: Thread is independent, safe  
**Cons**: Uses extra memory for copies

### By Reference with std::ref

```cpp
int count = 10;
std::thread t([&count]() {  // Reference
    count++;  // Modifies original
}, std::ref(count));
t.join();
```

**Pros**: Efficient, can modify original  
**Cons**: Must ensure original lives longer than thread

### Example with std::ref

```cpp
void modifyValue(int& value) {
    value = 100;
}

int main() {
    int data = 5;
    std::thread t(modifyValue, std::ref(data));  // Pass by reference
    t.join();
    std::cout << data << "\n";  // Prints 100
}
```

## Thread IDs and Management

```cpp
#include <thread>
#include <iostream>

void printThreadInfo() {
    std::cout << "Thread ID: " << std::this_thread::get_id() << "\n";
}

int main() {
    std::cout << "Main thread ID: " << std::this_thread::get_id() << "\n";

    std::thread t1(printThreadInfo);
    std::thread t2(printThreadInfo);

    std::cout << "t1 ID: " << t1.get_id() << "\n";
    std::cout << "t2 ID: " << t2.get_id() << "\n";

    t1.join();
    t2.join();
}
```

## Comparison Table

| Method     | Syntax                     | State    | Use Case          | Complexity |
| ---------- | -------------------------- | -------- | ----------------- | ---------- |
| Function   | `std::thread(func)`        | No       | Simple operations | Low        |
| Lambda     | `std::thread([](){})`      | Optional | Quick tasks       | Low        |
| Functor    | `std::thread(Obj())`       | Yes      | Complex logic     | Medium     |
| Non-static | `std::thread(&C::f, &obj)` | Yes      | Member functions  | Medium     |
| Static     | `std::thread(&C::f)`       | No       | Utility functions | Low        |

## Best Practices

✅ **Prefer lambdas** for new code (cleaner, more readable)  
✅ **Use std::ref** when you intentionally need references  
✅ **Copy small data** (ints, strings) by value  
✅ **Be aware of lifetimes** - ensure referenced objects survive  
✅ **Keep thread work focused** - one task per thread  
✅ **Always join or detach** before thread object is destroyed

## Common Mistakes

### Mistake 1: Reference to Temporary

```cpp
// ❌ Wrong
std::thread t([](int& x) { x++; }, 5);  // 5 is temporary

// ✅ Correct
int value = 5;
std::thread t([](int& x) { x++; }, std::ref(value));
```

### Mistake 2: Forgetting to Join

```cpp
// ❌ Wrong - program may terminate
std::thread t(work);
// Thread destroyed without join/detach

// ✅ Correct
std::thread t(work);
t.join();  // or t.detach()
```

### Mistake 3: Accessing Destroyed Objects

```cpp
// ❌ Wrong
std::thread createThread() {
    int local = 10;
    return std::thread([&local]() {
        std::cout << local;  // local destroyed!
    });
}
```

## Next Steps

- Learn about `join()` vs `detach()`: `02_join_detach.cpp`
