

#include <iostream>
#include "task.h"


task print() {
	for (int i = 1; i <= 10; ++i) {
		std::cout << i << '\n';
		co_await std::suspend_always();
	}
}


int main() {
	auto f = print();
	std::cout << "init point\n";
	f.resume();
	std::cout << "one\n";
	f.resume();
	std::cout << "second\n";
	f.resume();
	std::cout << "three\n";
	std::cout << "CANCEL...\n";
	f.cancel();

	/*
	 *      init point
			1
			one
			2
			second
			3
			three
			CANCEL...
			4
			5
			6
			7
			8
			9
			10
	 */
}
