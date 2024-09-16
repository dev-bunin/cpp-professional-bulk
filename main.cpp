#include <iostream>
#include <sstream>

#include "commandhandler.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		// Не верное кол-во аргументов
		return 1;
	}

	parseCommand(atoi(argv[1]), std::cin);
	return 0;
}
