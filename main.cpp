
#include <QCoreApplication>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <chrono>


int products{0};
int capacity{50};
std::condition_variable cv;
std::mutex mtx;

void produce() {
    while(products < capacity) {
        std::unique_lock<std::mutex> lock(mtx);
        products++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate work
        std::cout<<"produced products = "<<products<<std::endl;
        if(products == 10) {
            cv.notify_one();
            break;
        }
    }
}

void consume() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{return products == 10;});
    while(products > 0) {
        products--;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate work
        std::cout<<"consumed products = "<<products<<std::endl;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::thread myProducer{produce};
    std::thread myConsumer{consume};
    myProducer.join();
    myConsumer.join();
    return a.exec();
}
