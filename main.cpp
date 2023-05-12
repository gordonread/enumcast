#include "Enum.h"

#include <iostream>

enum Fruit : int16_t { Apple=4, Pear, Banana=9, MinValue=-1, MaxValue=11, UnknownValue=0};

enum class Light : char { Red = 'R', Amber='A', Green='G'};

int main() {
    
    auto name_string = enum_cast<std::string>(Fruit::Apple);
    auto name_string2 = enum_cast<std::string>(Fruit::Banana);
    std::cout << name_string << " " << name_string2 << std::endl;
    std::cout << enum_cast<Fruit>("Apple") << " " << enum_cast<Fruit>("Banana") << " " << enum_cast<Fruit>("Orange") << std::endl;
    std::cout << enum_cast<std::string>(enum_cast<Fruit>("Apple")) << " " << enum_cast<std::string>(enum_cast<Fruit>("Banana")) << " " << enum_cast<std::string>(enum_cast<Fruit>("Orange")) << std::endl;


    std::cout << "Red [" << static_cast<char>(enum_cast<Light>("Red")) << "]" << std::endl;
    std::cout << "Amber [" << static_cast<char>(enum_cast<Light>("Amber")) << "]" << std::endl;
    std::cout << "Green [" << static_cast<char>(enum_cast<Light>("Green")) << "]" << std::endl;

    std::cout << "Red [" << enum_cast<std::string>(Light::Red) << "]" << std::endl;
}
