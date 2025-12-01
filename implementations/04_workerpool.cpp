#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <sstream>
using namespace std;

void init(){
#define workersCount 2
}

class ThreadPool
{
private:
    bool stop;

    vector<thread> workers;
    queue<function<void()>> jobs;

    mutex mu;
    condition_variable cv;

    /**
     *  Worker_loop: Function run by each worker thread
     *  Continuously fetches jobs from the queue and executes them
     */
    void worker_loop()
    {
        // Runs until pool is closed or jobs are done
        while (true)
        {
            function<void()> job;

            // IMP: acquire the mutex to access the queue
            std::unique_lock<mutex> lock(mu);

            // Wait till there is a job in the pool or bool is being stopped
            cv.wait(lock, [this]
                    { return stop || !jobs.empty(); });

            if (stop && jobs.empty())
                return;

            // fetch/move the job
            job = std::move(jobs.front());
            jobs.pop();

            // IMP: Unlock the mutex before executing the job so other threads can access the queue
            lock.unlock();

            // execute the job
            job();
        }
    }

public:
    /**
     *  IMP: Instantitate the pool with n workers and stop = false
     */
    ThreadPool(int numOfWorkers) : stop(false)
    {
        for (int i = 0; i < numOfWorkers; i++)
        {
            // for each thread, we call the worker_loop function
            workers.emplace_back([this]
                                 { worker_loop(); });
        }
    }

    void enqueue(function<void()> job)
    {
        /**
         *  IMP:
         *  Extra scope {} is used to destroy the mutex before notifying to other threads
         *  out of scope
         */
        {
            // Lock acquired
            lock_guard<mutex> lock(mu);

            // Critical section
            jobs.push(std::move(job));
        }

        // Notify other threads that the job is avaiable
        cv.notify_one();
    }

    ~ThreadPool()
    {
        /** IMP: Extra scope {} since we need to make sure that the mutex is unlocked before notifying
         *       other threads
         */
        {
            lock_guard<mutex> lock(mu);
            stop = true;
        }

        // Wake up the threads
        cv.notify_all();

        for (auto &w : workers)
            if (w.joinable())
                w.join();
    }
};

int main()
{
    ThreadPool pool(workersCount);

    cout << "Thread Pool initialized with " << workersCount << " workers\n";
    cout << "Adding jobs to the queue...\n";

    for (int jobId = 0; jobId < 10; jobId++)
    {
        pool.enqueue([jobId]
                     {
            cout << "Job " << jobId << " executed by thread by " << this_thread::get_id() << "\n";
            this_thread::sleep_for(1s); });
    }
}