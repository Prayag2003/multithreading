#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

int balance = 0;
condition_variable cv;
mutex mut;

void withDrawMoney(int amount)
{
    cout << "Inside Withdraw Money\n";
    unique_lock<mutex> ul(mut);

    // wait for other thread based on a condition
    cout << "Waiting for the condition to get updated\n";
    cout << "Wait releases the mutex and waits upon the condition\n";
    cv.wait(ul, []
            { return (balance != 0) ? true : false; });

    if (balance >= amount)
    {
        balance -= amount;
        cout << "Amount deducted: " << amount << "\n";
    }
    else
    {
        cout << "Insufficient balance, current balance is less than " << amount << "\n";
    }
    cout << "Balance: " << balance << "\n";
}

void addMoney(int amount)
{
    cout << "Inside Add Money\n";
    lock_guard<mutex> lg(mut);
    balance += amount;
    cout << "Amount added " << amount << "\n";
    cout << "Balance = " << balance << "\n";

    // NOTE: Important to notify the other thread after the work has been done
    cv.notify_one();
}

int main()
{
    thread withDrawMoneyThread(withDrawMoney, 500);
    thread addMoneyThread(addMoney, 500);

    withDrawMoneyThread.join();
    addMoneyThread.join();

    return 0;
}