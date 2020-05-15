#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using std::cout;
using std::cin;
using std::thread;
using std::string;
using std::endl;

int stopflag = 0;

void input_func()
{
    while(true && !stopflag)
    {
        string input;
        cin >> input;
        cout << "Input: " << input << endl;
    }
}

void output_func()
{
    while(true && !stopflag)
    {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        cout << "Output thread\n";
    }
}

int main()
{
    thread inp(input_func);
    thread outp(output_func);

    std::this_thread::sleep_for (std::chrono::seconds(5));
    stopflag = 1;
    outp.join();
    cout << "Joined output thread\n";
    inp.join();

    cout << "End of main, all threads joined.\n";

    return 0;
}

// #include <thread>
// #include <iostream>
// #include <future>

// void do_a(std::atomic_bool& done) {
//      // do things
//      done = true;
// }

// int main() {
//     std::atomic_bool done = false;

//     auto a = std::async(do_a, done);

//     auto b = std::async([](std::atomic_bool& done) {
//         while (not done) {
//             std::cout << "still not done" << std::endl;
//             sleep(1);
//         }
//     });

//     // other stuff in parallel.
// }