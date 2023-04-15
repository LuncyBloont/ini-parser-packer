#include <fstream>
#include <iostream>
#include <inipp/inipp.hpp>

int main()
{
    std::ifstream fin("./test0.ini");

    inipp::Ini ini{};

    fin >> ini;

    std::cout << ini << std::endl;

    std::cout << ini["Koko"]["name"] << std::endl;
    std::cout << ini["Apple"]["name"] << std::endl;
    std::cout << ini["Monkey Park"]["headers"] << std::endl;
    
    return 0;
}
