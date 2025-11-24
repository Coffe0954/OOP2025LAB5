#include <iostream>
#include "fixed_block_memory_resource.h"
#include "queue.h"

// сложная структура для демонстрации
struct Person {
    std::string name;
    int age;
    double salary;

    Person(const std::string& n = "", int a = 0, double s = 0.0) 
        : name(n), age(a), salary(s) {}

    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        os << "person{name: " << p.name << ", age: " << p.age << ", salary: " << p.salary << "}";
        return os;
    }
};

void demonstrate_int_queue() {
    std::cout << "=== demonstration of queue with int ===" << std::endl;
    
    FixedBlockMemoryResource mr(1024); // 1kb memory pool
    Queue<int> queue(&mr);

    // добавляем элементы
    for (int i = 1; i <= 5; ++i) {
        queue.push(i * 10);
        std::cout << "added: " << i * 10 << std::endl;
    }

    std::cout << "\nqueue size: " << queue.size() << std::endl;
    std::cout << "first element: " << queue.front() << std::endl;
    std::cout << "last element: " << queue.back() << std::endl;

    // обход с помощью итератора
    std::cout << "\niteration with iterator:" << std::endl;
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // удаляем элементы
    std::cout << "\nremoving elements:" << std::endl;
    while (!queue.empty()) {
        std::cout << "removed: " << queue.front() << std::endl;
        queue.pop();
    }

    std::cout << "queue is empty: " << (queue.empty() ? "yes" : "no") << std::endl;
}

void demonstrate_person_queue() {
    std::cout << "\n=== demonstration of queue with person ===" << std::endl;
    
    FixedBlockMemoryResource mr(2048); // 2kb memory pool
    Queue<Person> queue(&mr);

    // добавляем сложные объекты
    queue.push(Person("ivan", 30, 50000.0));
    queue.push(Person("maria", 25, 60000.0));
    queue.push(Person("petr", 35, 70000.0));

    std::cout << "queue size: " << queue.size() << std::endl;

    // обход с помощью итератора
    std::cout << "\nqueue contents:" << std::endl;
    for (const auto& person : queue) {
        std::cout << person << std::endl;
    }

    // модификация через итератор
    std::cout << "\nmodification through iterator:" << std::endl;
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        it->salary += 1000; // увеличиваем зарплату
        std::cout << *it << std::endl;
    }

    // очистка
    while (!queue.empty()) {
        std::cout << "removed: " << queue.front().name << std::endl;
        queue.pop();
    }
}

void demonstrate_memory_reuse() {
    std::cout << "\n=== demonstration of memory reuse ===" << std::endl;
    
    FixedBlockMemoryResource mr(512); // маленький пул для демонстрации
    Queue<int> queue(&mr);

    // заполняем очередь
    for (int i = 0; i < 3; ++i) {
        queue.push(i);
    }
    std::cout << "added 3 elements. size: " << queue.size() << std::endl;

    // освобождаем память
    while (!queue.empty()) {
        queue.pop();
    }
    std::cout << "all elements removed. size: " << queue.size() << std::endl;

    // добавляем снова - память должна переиспользоваться
    for (int i = 10; i < 13; ++i) {
        queue.push(i);
    }
    std::cout << "added 3 new elements. size: " << queue.size() << std::endl;

    std::cout << "contents: ";
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

int main() {
    demonstrate_int_queue();
    demonstrate_person_queue();
    demonstrate_memory_reuse();

    std::cout << "\n=== all tests completed ===" << std::endl;
    return 0;
}