
#include <iostream>
#include <vector>
#include "sequence.h"

int main()
{
	Sequence<double> a(5, 1);
	a[3] = 3;
	std::cout << "a = " << a << std::endl;

	Sequence<double> b(10, 2, -1);
	std::cout << "b = " << b << std::endl;

	std::cout << "a * b = " << (a * b) << std::endl;

	return 0;
}