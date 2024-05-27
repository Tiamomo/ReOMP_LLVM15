#include <iostream>
#include <thread>

void test_func(int* a) {
    *a = 1;
}

int main() {
    int arr[2];
    std::thread t1(test_func, &arr[0]);
    std::thread t2(test_func, &arr[1]);
    t1.join();
    t2.join();
    return 0;
}

