#include <iostream>

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    int num;
    std::cout << "Enter the number of terms: ";
//    std::cin >> num;

//    std::cout << "Fibonacci Series: ";
//    for (int i = 0; i < num; i++) {
//        std::cout << fibonacci(i) << " ";
//    }
 //   std::cout << std::endl;

    return 0;
}

