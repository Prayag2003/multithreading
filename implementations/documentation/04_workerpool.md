# Worker Pool Pattern 🏭

## Overview

A **Worker Pool** (also called Thread Pool) is a concurrency pattern where:

- A fixed number of **worker threads** are pre-created and waiting for tasks
- **Tasks are submitted** to a shared queue by a main thread or other producers
- **Workers continuously fetch** tasks from the queue and execute them
- **Threads are reused** across multiple tasks, reducing creation/destruction overhead

This pattern is highly efficient for scenarios with many short-lived tasks.

### Standard Definition

The Worker Pool pattern implements a queue of tasks with a fixed number of threads:

- Fixed number of worker threads (pool size)
- Shared task queue (usually thread-safe)
- Workers wait for tasks when queue is empty
- Tasks are distributed among available workers
- Pool gracefully shuts down after all tasks complete

## Why Worker Pool?

### Problem: Creating Threads Per Task

**❌ Inefficient approach:**

```cpp
// One thread per task - WASTEFUL
for (int i = 0; i < 1000000; i++) {
    std::thread t([]() {
        // Do some work
        doWork();
    });
    t.join();  // Wait for thread to finish
}
```

**Problems:**

- Creating 1,000,000 threads is extremely expensive
- Thread creation/destruction overhead dominates execution time
- Resource exhaustion (memory, file descriptors, kernel threads)
- Unpredictable performance

### Solution: Worker Pool

**✅ Efficient approach:**

```cpp
// Fixed pool of workers, reused for all tasks
ThreadPool pool(4);  // 4 worker threads

for (int i = 0; i < 1000000; i++) {
    pool.enqueue([]() {
        doWork();
    });
}
```

**Benefits:**

- Threads are created once, reused many times
- Minimal overhead per task (just queue insertion)
- Controlled resource usage (fixed number of threads)
- Predictable and efficient performance

## Understanding the Code

The Worker Pool implementation consists of:

1. **ThreadPool class**: Manages workers and task queue
2. **Worker threads**: Execute tasks from the queue
3. **Task queue**: Stores pending tasks
4. **Synchronization**: Mutex and condition variables

### Basic Structure

```cpp
class ThreadPool {
private:
    // Configuration
    bool stop;

    // Workers
    vector<thread> workers;

    // Task storage
    queue<function<void()>> jobs;

    // Synchronization
    mutex mu;
    condition_variable cv;

    // Worker function
    void worker_loop() {
        while (true) {
            function<void()> job;

            std::unique_lock<mutex> lock(mu);
            cv.wait(lock, [this]() {
                return stop || !jobs.empty();
            });

            if (stop && jobs.empty())
                return;

            job = std::move(jobs.front());
            jobs.pop();
            lock.unlock();

            job();  // Execute outside lock
        }
    }

public:
    ThreadPool(int numWorkers) : stop(false) {
        for (int i = 0; i < numWorkers; i++) {
            workers.emplace_back([this]() { worker_loop(); });
        }
    }

    void enqueue(function<void()> task) {
        {
            lock_guard<mutex> lock(mu);
            jobs.push(std::move(task));
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        {
            lock_guard<mutex> lock(mu);
            stop = true;
        }
        cv.notify_all();

        for (auto& w : workers) {
            if (w.joinable())
                w.join();
        }
    }
};
```

## How It Works

### Initialization Phase

```
Creating ThreadPool with 4 workers:

┌─────────────────┐
│  ThreadPool(4)  │
└────────┬────────┘
         │
    Create 4 workers
         │
    ┌────┴────┬─────┬─────┐
    ▼         ▼     ▼     ▼
  [W1]     [W2]  [W3]  [W4]
   │        │     │     │
   └────────┴─────┴─────┘
   All waiting for jobs
```

### Task Submission Phase

```
Main thread:                    Worker threads:
─────────────                   ────────────────
enqueue(task1)
enqueue(task2)
enqueue(task3)   ──────────→   ┌──────────────┐
enqueue(task4)                 │  Task Queue  │
enqueue(task5)                 │  [1][2][3]   │
                               │  [4][5]      │
                               └──────┬───────┘
                                      │
                        ┌─────────────┼─────────────┐
                        ▼             ▼             ▼
                       [W1]          [W2]          [W3]
                     Execute       Execute       Execute
                     task1         task2         task3
```

### Execution Flow Per Worker

```
┌──────────────────┐
│  Worker Loop     │
└────────┬─────────┘
         │
         ▼
    ┌─────────────────────┐
    │ Acquire lock        │
    │ Wait for task or    │◄─── Blocked if queue empty
    │ shutdown signal     │     and not stopping
    │ Release lock        │
    └────────┬────────────┘
             │
             ▼
         ┌──────────────┐
         │ Check stop & │
         │ empty queue? │─────► Yes ─► Return (exit)
         └──────┬───────┘
                │ No
                ▼
         ┌──────────────┐
         │ Fetch task   │
         │ Release lock │
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │ Execute task │◄─── No lock held
         │ (do work)    │     Other threads can
         └──────┬───────┘     access queue
                │
                ▼
            Loop back
```

## Key Components

### 1. Worker Threads

```cpp
vector<thread> workers;

// Created in constructor
for (int i = 0; i < numOfWorkers; i++) {
    workers.emplace_back([this]() { worker_loop(); });
}
```

- Fixed number determined at creation
- Each runs the `worker_loop()` function
- Lambda captures `this` to access member variables

### 2. Task Queue

```cpp
queue<function<void()>> jobs;
```

- Stores pending tasks (lambdas or function objects)
- Generic `function<void()>` accepts any callable
- Protected by mutex for thread-safety

### 3. Condition Variable

```cpp
condition_variable cv;

// In worker_loop:
cv.wait(lock, [this]() {
    return stop || !jobs.empty();
});

// In enqueue:
cv.notify_one();  // Wake one worker
```

- Workers wait when queue is empty
- Main thread notifies when task is added
- Avoids busy-waiting

### 4. Stop Signal

```cpp
bool stop;

// In destructor:
{
    lock_guard<mutex> lock(mu);
    stop = true;
}
cv.notify_all();  // Wake all workers
```

- Signals workers to exit gracefully
- Combined with `jobs.empty()` check
- Allows completion of remaining tasks

## Typical Usage Pattern

```cpp
// 1. Create pool with N workers
ThreadPool pool(4);

// 2. Submit tasks
for (int i = 0; i < 100; i++) {
    pool.enqueue([i]() {
        cout << "Task " << i << " executed\n";
        this_thread::sleep_for(100ms);
    });
}

// 3. Destructor waits for all tasks to complete
// ~ThreadPool() called when pool goes out of scope
```

### Output Characteristics

```
Task 0 executed by thread 139894520919808
Task 2 executed by thread 139894529512192
Task 1 executed by thread 139894520919808
Task 3 executed by thread 139894529512192
...
```

**Note:** Task order is NOT guaranteed! Tasks are processed by available workers, so ordering depends on scheduling.

## Execution Timeline Example

**Pool with 2 workers, 5 tasks:**

```
Time    Main        Worker1         Worker2         Queue
────────────────────────────────────────────────────────────
T0      create()    wait            wait            []
T1      enqueue(1)  wake            wait            [1]
T2                  fetch(1)        wait            []
        enqueue(2)  execute(1)      wait            [2]
T3                  execute(1)      fetch(2)        []
        enqueue(3)  execute(1)      execute(2)      [3]
T4                  done→wait       execute(2)      [3]
        enqueue(4)  fetch(3)        execute(2)      [4]
T5                  execute(3)      done→wait       [4]
        enqueue(5)  execute(3)      fetch(4)        [5]
T6                  execute(3)      execute(4)      [5]
        (all tasks                   execute(4)      [5]
        enqueued)
T7                  done→wait       done→wait       [5]
                    fetch(5)        (idle)          []
T8                  execute(5)                      []
T9                  done→wait       wait (idle)     []
```

## Advantages & Disadvantages

### Advantages ✅

| Advantage             | Benefit                                     |
| --------------------- | ------------------------------------------- |
| **Reuse threads**     | Avoid expensive creation/destruction        |
| **Bounded resources** | Fixed number of threads, predictable memory |
| **Scalability**       | Handles thousands of tasks efficiently      |
| **Responsiveness**    | Tasks start quickly (workers always ready)  |
| **Decoupling**        | Submitter doesn't wait for execution        |
| **Task ordering**     | Queue ensures FIFO processing               |

### Disadvantages ❌

| Disadvantage          | Mitigation                        |
| --------------------- | --------------------------------- |
| **Fixed pool size**   | Choose based on workload analysis |
| **No task priority**  | Use priority queue instead        |
| **No cancellation**   | Wrap in cancellation token        |
| **No return values**  | Use futures/promises              |
| **No dynamic sizing** | Use scalable pool implementation  |

## Compared to Alternatives

### Option 1: Creating Thread Per Task

```cpp
// Create new thread for each task
for (int i = 0; i < 1000; i++) {
    std::thread t([i]() { doWork(i); });
    t.join();
}
```

**❌ Problems:** Slow, resource waste, unpredictable

### Option 2: Worker Pool (This Pattern)

```cpp
// Reuse fixed pool of threads
ThreadPool pool(4);
for (int i = 0; i < 1000; i++) {
    pool.enqueue([i]() { doWork(i); });
}
```

**✅ Benefits:** Fast, efficient, predictable

### Option 3: Async (if return values needed)

```cpp
// For tasks with return values
std::vector<std::future<int>> futures;
for (int i = 0; i < 1000; i++) {
    futures.push_back(std::async(std::launch::async,
                                 []() { return doWork(); }));
}
for (auto& f : futures) {
    int result = f.get();
}
```

**Note:** See `fundamentals/12_async.cpp` for details

## Real-World Applications

### 1. Web Server

```cpp
// Accept connections and enqueue request handlers
ThreadPool pool(20);

while (true) {
    Socket client = server.accept();
    pool.enqueue([client]() {
        handleRequest(client);
    });
}
```

### 2. Task Processing

```cpp
// Process items from a database
ThreadPool pool(4);

for (auto& item : database.getItems()) {
    pool.enqueue([item]() {
        processItem(item);
    });
}
```

### 3. Image Processing

```cpp
// Process multiple images in parallel
ThreadPool pool(8);

for (auto& image : images) {
    pool.enqueue([image]() {
        image.resize();
        image.filter();
        image.save();
    });
}
```

### 4. I/O Operations

```cpp
// Read multiple files concurrently
ThreadPool pool(4);

for (auto& filename : filenames) {
    pool.enqueue([filename]() {
        processFile(filename);
    });
}
```

## Important Design Decisions

### 1. Lock Scope

```cpp
void enqueue(function<void()> task) {
    {
        lock_guard<mutex> lock(mu);  // Scope matters!
        jobs.push(std::move(task));
    }  // Lock released BEFORE notifying
    cv.notify_one();  // No lock held during notify
}
```

**Why?** Better performance - notification outside critical section

### 2. Unlock Before Execution

```cpp
void worker_loop() {
    while (true) {
        function<void()> job;
        {
            unique_lock<mutex> lock(mu);
            cv.wait(lock, [this]() {
                return stop || !jobs.empty();
            });
            if (stop && jobs.empty()) return;
            job = std::move(jobs.front());
            jobs.pop();
        }  // Lock released here

        job();  // Execute WITHOUT lock
    }
}
```

**Why?** Allows other workers to access queue during task execution

### 3. Move Semantics

```cpp
jobs.push(std::move(task));      // Efficient (no copy)
job = std::move(jobs.front());   // Efficient transfer
```

**Why?** Lambdas may be large; moving avoids expensive copies

## Extending the Pattern

### With Return Values (using futures)

```cpp
template<typename F>
auto enqueue(F f) -> std::future<decltype(f())> {
    auto task = std::make_shared<std::packaged_task<decltype(f())()>>(f);
    auto res = task->get_future();
    {
        std::lock_guard<std::mutex> lock(mu);
        jobs.push([task]() { (*task)(); });
    }
    cv.notify_one();
    return res;
}

// Usage:
auto result = pool.enqueue([](){ return 42; });
cout << result.get();  // Get the return value
```

### With Dynamic Pool Size

```cpp
void setPoolSize(int newSize) {
    {
        lock_guard<mutex> lock(mu);
        // Add or remove workers
    }
    cv.notify_all();
}
```

### With Task Priorities

```cpp
priority_queue<pair<int, function<void()>>> jobs;  // int = priority

void enqueue(function<void()> task, int priority = 0) {
    {
        lock_guard<mutex> lock(mu);
        jobs.push({priority, std::move(task)});
    }
    cv.notify_one();
}
```

## Common Patterns

### Pattern 1: Fire and Forget

```cpp
// Submit task, don't wait for result
pool.enqueue([]() { backgroundWork(); });
```

### Pattern 2: Wait for All Tasks

```cpp
// Enqueue multiple tasks
for (int i = 0; i < 100; i++) {
    pool.enqueue([i]() { processTask(i); });
}

// Wait implicitly when destructor called
}  // ~ThreadPool() waits for all
```

### Pattern 3: Batch Processing

```cpp
// Process items in batches
ThreadPool pool(4);
for (auto& batch : batches) {
    pool.enqueue([batch]() {
        for (auto& item : batch) {
            process(item);
        }
    });
}
```

## Best Practices

✅ **Choose pool size wisely** - CPU-bound: #cores, I/O-bound: 2x-4x #cores  
✅ **Move tasks into queue** - Avoid copying expensive lambdas  
✅ **Execute outside locks** - Let other threads access queue  
✅ **Use unique_lock for wait()** - Required for condition variables  
✅ **Notify after unlock** - Better performance  
✅ **Test graceful shutdown** - Verify all tasks complete

## Common Mistakes

### Mistake 1: Waiting for Specific Task

```cpp
// ❌ Wrong - can't track individual tasks
pool.enqueue(task);
// No way to know when THIS task is done
```

**Solution:** Use futures/promises (extended pattern)

### Mistake 2: Holding Lock During Execution

```cpp
// ❌ Wrong - serializes all tasks
{
    lock_guard<mutex> lock(mu);
    job = jobs.front();
    jobs.pop();
    job();  // Holding lock! Other workers blocked
}
```

**Solution:** Release lock before executing

### Mistake 3: Not Handling Shutdown

```cpp
// ❌ Wrong - destructor blocks forever
~ThreadPool() {
    // Forgot to set stop flag
    for (auto& w : workers)
        w.join();  // Threads never exit!
}
```

**Solution:** Set stop flag and notify all before joining

## Performance Considerations

### Pool Size Selection

```
CPU-bound tasks:        pool_size = std::thread::hardware_concurrency()
I/O-bound tasks:        pool_size = 2 * std::thread::hardware_concurrency()
Mixed workload:         pool_size = std::thread::hardware_concurrency() + 2
```

### Benchmarking

```cpp
auto start = high_resolution_clock::now();

ThreadPool pool(4);
for (int i = 0; i < 10000; i++) {
    pool.enqueue([]() { doQuickWork(); });
}

auto end = high_resolution_clock::now();
auto duration = duration_cast<milliseconds>(end - start);
cout << "Completed 10000 tasks in " << duration.count() << "ms\n";
```

## Next Steps

- Learn futures and promises: `fundamentals/11_future_and_promise.cpp`
- Learn async: `fundamentals/12_async.cpp`
- Implement priority queue version: Extend this implementation
- Add cancellation support: Use atomic flags

## References

- **Pattern name:** Thread Pool / Worker Pool
- **Use case:** Task-based parallelism
- **Synchronization:** Mutex + Condition Variable
- **Standard library:** `std::thread`, `std::function`, `std::queue`
