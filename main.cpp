#include <iostream>
#include <memory>

class Testclass {
public:
    Testclass(int a, int b) {}
};
int main() {
    std::cout << "Hello, World!" << std::endl;
    auto test = new Testclass(1, 1);
    auto ptr = std::make_shared<Testclass>(1, 1);
    auto nullp = std::shared_ptr<Testclass>(nullptr);
    return 0;
}
