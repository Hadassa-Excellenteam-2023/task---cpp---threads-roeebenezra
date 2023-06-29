#include <iostream>
#include <thread>
#include <vector>
#include <future>

std::mutex mtx;
std::condition_variable cv;
bool finished = false;

class Producer {
private:
    std::thread::id thread_id;
    double value;

public:
    Producer(std::thread::id id, double val) : thread_id(id), value(val) {}

    void operator()() {
        std::unique_lock<std::mutex> lock(mtx);
        while (value != 0) {
            std::cout << std::hash<std::thread::id>{}(thread_id) << " sent: " << value << std::endl;
            value /= 10;
        }
        std::cout << std::hash<std::thread::id>{}(thread_id) << " finished" << std::endl;

        if (finished) {
            cv.notify_one();
        }
        else {
            finished = true;
            cv.wait(lock);
            cv.notify_one();
        }
    }
};


int main() {
    std::vector<std::future<void>> producers;
    std::thread consumer([]() {});

    for (int i = 0; i < 3; ++i) {
        producers.emplace_back(std::async(std::launch::async, Producer(std::this_thread::get_id(), 28312)));
        producers.emplace_back(std::async(std::launch::async, Producer(std::this_thread::get_id(), 38348)));
        producers.emplace_back(std::async(std::launch::async, Producer(std::this_thread::get_id(), 99341)));
    }

    consumer.detach();

    for (auto& producer : producers) {
        try {
            producer.get();
        }
        catch (const std::exception& e) {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }
    }

    return 0;
}
