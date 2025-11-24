/* Thread is a light weight process used to achieve parallelism by dividing a process to multiple threads

Eg:
1) Browser having multiple tabs
2) MS word uses multiple threads, one for format, other for spellcheck

NOTE: The thread execution order is not defined, it can be any order

*/

#include<iostream>
#include<thread>
#include<chrono>
#include<algorithm>
using namespace std;
using namespace std::chrono;
typedef unsigned long long ull;

ull oddSum = 0, evenSum = 0;

void findEven(ull start, ull end){
	for(ull i = start; i <= end; i++){
		if(!(i & 1)){
			evenSum += i;
		}
	}
}

void findOdd(ull start, ull end){
	for(ull i = start; i <= end; i++){
		if(i & 1){
			oddSum += i;
		}
	}
}
 
int main(){
	ull start = 0, end = 1900000000;
	auto startTime = high_resolution_clock::now();

	// multi-threading
	// Create thread using Function Pointers
	std::thread t1(findOdd, start, end);
	std::thread t2(findEven, start, end);

	// findOdd(start, end);	
	// findEven(start, end);
	t1.join();
	t2.join();

	auto stopTime = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stopTime - startTime);

	cout << "Odd Sum " << oddSum << "\n";
	cout << "Even Sum " << evenSum << "\n";

	cout << "Time taken is " << duration.count() << "\n";
	return 0;
}