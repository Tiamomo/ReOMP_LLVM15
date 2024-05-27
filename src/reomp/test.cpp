// test.cpp
#include <iostream>

void foo() {
  std::cout << "In foo" << std::endl;
}

void bar() {
  std::cout << "In bar" << std::endl;  
}

int main() {
  foo();
  bar();
  return 0;
}
