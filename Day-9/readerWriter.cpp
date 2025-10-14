#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

using namespace std;

mutex resource_mutex;
mutex reader_count_mutex;
mutex io_mutex;
int shared_data = 0;
int reader_count = 0;

mt19937 rng;

class Reader {
private:
    int id;

public:
    Reader(int i) : id(i) {}

    void operator()() {
        while (true) {
            {
                lock_guard<mutex> lock(reader_count_mutex);
                reader_count++;
                if (reader_count == 1) {
                    resource_mutex.lock();
                }
            }

            {
                lock_guard<mutex> lock(io_mutex);
                cout << "Reader " << id << " is reading data: " << shared_data << " (Total readers: " << reader_count << ")" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(rng() % 1500 + 500));

            {
                lock_guard<mutex> lock(reader_count_mutex);
                reader_count--;
                if (reader_count == 0) {
                    resource_mutex.unlock();
                }
            }

            this_thread::sleep_for(chrono::milliseconds(rng() % 2000 + 1000));
        }
    }
};

class Writer {
private:
    int id;

public:
    Writer(int i) : id(i) {}

    void operator()() {
        while (true) {
            int new_value;
            {
                lock_guard<mutex> lock(io_mutex);
                cout << ">>> Writer " << id << ", enter new value: ";
            }

            unique_lock<mutex> lock(resource_mutex, defer_lock);

            cin >> new_value;

            lock.lock();
            shared_data = new_value;
            {
                lock_guard<mutex> lock(io_mutex);
                cout << ">>> Writer " << id << " wrote data: " << shared_data << " <<<" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(rng() % 800 + 500));
            lock.unlock();

            this_thread::sleep_for(chrono::milliseconds(rng() % 1500 + 1000));
        }
    }
};

int main() {
    int num_readers, num_writers;
    cout << "Enter the number of readers: ";
    cin >> num_readers;
    cout << "Enter the number of writers: ";
    cin >> num_writers;

    rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count());

    cout << "Readers-Writers Simulation Started." << endl;

    int total_threads = num_readers + num_writers;
    Reader** readers = new Reader*[num_readers];
    Writer** writers = new Writer*[num_writers];
    thread* threads = new thread[total_threads];
    int thread_index = 0;

    for (int i = 0; i < num_readers; ++i) {
        readers[i] = new Reader(i + 1);
        threads[thread_index++] = thread(ref(*readers[i]));
    }

    for (int i = 0; i < num_writers; ++i) {
        writers[i] = new Writer(i + 1);
        threads[thread_index++] = thread(ref(*writers[i]));
    }

    for (int i = 0; i < total_threads; ++i) {
        threads[i].join();
    }

    for (int i = 0; i < num_readers; ++i) {
        delete readers[i];
    }
    for (int i = 0; i < num_writers; ++i) {
        delete writers[i];
    }
    delete[] readers;
    delete[] writers;
    delete[] threads;

    return 0;
}