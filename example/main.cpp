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
    std::cout << ini.get("Koko", "set") << std::endl;
    std::cout << ini.get("apple", "name") << std::endl;

    for (const auto& e : ini["Monkey Park"]["headers"].asStrArr(","))
    {
        std::cout << e << "; ";
    }
    std::cout << std::endl;

    for (const auto& e : ini["Monkey Park"]["age"].asInt64Arr("-"))
    {
        std::cout << e << "; ";
    }
    std::cout << std::endl;

    for (const auto& e : ini["Monkey Park"]["height"].asFloatArr("\\|-\\|"))
    {
        std::cout << e << "; ";
    }
    std::cout << std::endl;

    inipp::Ini ini2;

    std::ifstream("./test0.ini") >> ini2;

    std::cout << "\nini2: " << std::endl;
    std::cout << ini2;
    
    return 0;
}
