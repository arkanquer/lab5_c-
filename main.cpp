#include <iostream>
#include <string>
#include <syncstream>
#include <future>
#include <thread>
#include <chrono>

using namespace std;

void calculate(const std::string& name, int seconds)
{
    this_thread::sleep_for(chrono::seconds(seconds));
    osyncstream(std::cout) << name << endl;
}

void quick(const string& name)
{
    calculate(name, 1);
}
void slow(const string& name)
{
    calculate(name, 7);
}

void work()
{
    using clock = chrono::steady_clock;

    auto begin = clock::now();

    promise<void> a1_done_promise;
    promise<void> a3_done_promise;

    future<void> a1_done_future = a1_done_promise.get_future();
    future<void> a3_done_future = a3_done_promise.get_future();

    auto worker1 = async(std::launch::async,
        [fA1 = std::move(a1_done_future),
          fA3 = std::move(a3_done_future)]()
        {
        slow("A2");
        fA1.wait();
        quick("B1");
        fA3.wait();
        quick("C1");
        }
    );

    auto worker2 = async(launch::async,[]()
        {
            slow("B2");
            quick("C2");
        }
    );

    quick("A1");
    a1_done_promise.set_value();

    quick("A3");
    a3_done_promise.set_value();

    worker1.get();
    worker2.get();

    auto end = clock::now();
    auto seconds =
        chrono::duration_cast<chrono::seconds>(end - begin).count();
    osyncstream(cout) << "time: " << seconds <<"seconds" << endl;
}

int main()
{
    osyncstream(cout) << "Processing the data" << endl;
    work();
    osyncstream(cout) << "Work is over!" << endl;
    return 0;
}