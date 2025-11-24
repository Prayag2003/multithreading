#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

/*
	JOIN:
	 1. We wait for the thread to finish its execution before the main returns
	 2. Double join === program termination
	 3. We check if the thread is joinnable using joinnable()
*/
void Join(int count)
{
	while (count-- > 0)
	{
		cout << "Hello " << "\n";
	}
	this_thread::sleep_for(chrono::seconds(3));
}

// int main(){
// 	thread t(Join, 5);
// 	cout << "main() " << "\n";
// 	t.join();

// 	if(t.joinable()) t.join();
// 	return 0;
// }

/*
	DETACH:
	 1. This is used to detach newly created thread from the parent thread;
	 2. Double detaach => termination
*/

int main()
{
	thread t(Join, 5);
	cout << "main() " << "\n";
	t.detach();

	if (t.joinable())
		t.detach();

	// t.detach(); => ERROR
	cout << "main() after" << "\n";

	return 0;
}