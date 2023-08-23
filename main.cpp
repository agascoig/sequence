
#include <iostream>
#include <vector>
#include "sequence.h"

int main()
{
	Sequence<double> a({ 0, 0, 0, 2, 0, 2, 0, 0 });
	std::cout << "a = " << a.trim() << std::endl;

	Sequence<double> b({ 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 });
	std::cout << "b = " << b.trim() << std::endl;

	std::cout << b + a << std::endl;

	return 0;
}