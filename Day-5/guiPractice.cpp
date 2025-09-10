#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

// sleep utility
void pauseMS(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

int main() {
    cout << "\033[1mBold Text\033[0m\n";
    pauseMS(1000);
    cout << "\033[2mDim Text\033[0m\n";
    pauseMS(1000);
    cout << "\033[38;5;39mRed Text\033[0m\n";
    pauseMS(1000);
    cout << "\033[2J\033[H";   // clear screen
    cout << "Screen Cleared!\n";
cout << "\033[1mBold Text ";
cout << "Ye bhi bold rahega ";
cout << "\033[38;5;39mAur ye bhi bold hai\n";
    return 0;
}
