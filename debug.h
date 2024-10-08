#pragma once
#include <iostream>
#define GREEN_TEXT "\033[1;32m"
#define RESET_TEXT "\033[0m"
#define DEBUG(x) do {std::cout <<"[" GREEN_TEXT "+" RESET_TEXT "] " << x << std::endl;char _; std::cin >> std::noskipws >> _ ;} while (0)
#define TODO() DEBUG("TODO!")



