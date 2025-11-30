#include <iostream>
#include <thread>

// Ways of Thread Creation

// 1. Function pointer
void fun(int x)
{
	while (x-- > 0)
	{
		std::cout << x << "\n";
	}
}

void usingFun()
{
	std::thread t(fun, 10);
	t.join();
}

// ------------------------------------
// 2. Lambda functions

void usingLambda()
{
	// auto fun = [](int x){
	// 	while(x-- > 0){
	// 		std::cout << x << "\n";
	// 	}
	// };

	std::thread t([](int x)
				  {
		while(x-- > 0){
			std::cout << x << "\n";
		} }, 10);
	t.join();
}

// ------------------------------------
// 3. Functor (function object)
class Base
{
public:
	void operator()(int x)
	{
		while (x-- > 0)
		{
			std::cout << x << "\n";
		}
	}
};

void usingFunctor()
{
	std::thread t(Base(), 10);
	t.join();
}

// ------------------------------------
// 4. Non Static Member function
class Base2
{
public:
	void trigger(int x)
	{
		while (x-- > 0)
		{
			std::cout << x << "\n";
		}
	}
};

void usingNonStaticMember()
{
	Base2 b2;
	// since non static depends on object and not class
	std::thread t(&Base2::trigger, &b2, 10);
	t.join();
}

class Base3
{
public:
	static void trigger(int x)
	{
		while (x-- > 0)
		{
			std::cout << x << "\n";
		}
	}
};

void usingStaticMember()
{
	std::thread t(&Base3::trigger, 10);
	t.join();
}

int main()
{
	usingFun();
	usingLambda();
	usingFunctor();
	usingNonStaticMember();
	usingStaticMember();
}
