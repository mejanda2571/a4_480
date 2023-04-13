#include <iostream>
#include <semaphore.h>

int main() {
    std::cout << "Hello, World!" << std::endl;

    // Create a semaphore
    sem_t semaphore;
    int result = 0;
    result = sem_init(&semaphore, 0, 1);
    std::cout << "Value of semaphore: " << result << std::endl;
    return 0;

}
