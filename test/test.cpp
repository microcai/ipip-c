
#include <iostream>

#include "ipip.hpp"

int main()
{
	ipip ipip("17monipdb.dat");

	std::cout << ipip.find(inet_addr("8.8.8.8")) << std::endl;
	std::cout << ipip.find(inet_addr("114.114.114.114")) << std::endl;
	return 0;
}
