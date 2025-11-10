
#include <QCoreApplication>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <chrono>
#include <queue>


int products{0};
int capacity{10};

//std::condition_variable cv_produce;
//std::condition_variable cv_consume;
std::condition_variable cv;
std::mutex mtx;
bool readyToConsumeFlag = false;
bool readyToProduceFlag = true;

class ServingLine {
public:
    std::queue<int> q;
public :
    void fillServingLine() {
        q.push(1);
    }
    void removeFromServingLine() {
        q.pop();
    }
};

ServingLine ourLine;

void produce() {
    while(products < capacity) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return readyToProduceFlag;});
        products++;
        ourLine.fillServingLine();

        std::cout<<"produced products = "<<products<<std::endl;
        if(!ourLine.q.empty()) {
            lock.unlock(); //this line is important, unlock the mutex for consumer to proceed
            cv.notify_one();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // play around with this parameter
    }
}

void consume() {
    int consumed = 0;
    while(consumed < capacity) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return !ourLine.q.empty();});
        consumed++;
        ourLine.removeFromServingLine();

        std::cout<<"consumed products = "<<consumed<<std::endl;
        if(ourLine.q.empty()) {
            lock.unlock();
            cv.notify_one();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250)); // Simulate work
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
