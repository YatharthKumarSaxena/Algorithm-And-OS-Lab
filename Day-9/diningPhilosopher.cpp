#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

using namespace std;

class Philosopher{
private:
    int id;
    mutex& left_fork;
    mutex& right_fork;
    mt19937 rng;

    void think(){
        cout<<"Philosopher "<<id<<" is thinking."<<endl;
        this_thread::sleep_for(chrono::milliseconds(rng() % 1000 + 500));
    }

    void eat(){
        cout<<"Philosopher "<<id<<" is EATING."<<endl;
        this_thread::sleep_for(chrono::milliseconds(rng() % 1000 + 500));
    }

public:
    Philosopher(int id, mutex& left, mutex& right): id(id), left_fork(left), right_fork(right){
        rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count() + id);
    }

    void dine(){
        for(int i=0;i<1;i++){
            think();
            if(&left_fork < &right_fork){
                lock_guard<mutex> left_lock(left_fork);
                cout<<"\tPhilosopher "<<id<<" picked up left fork."<<endl;
                this_thread::sleep_for(chrono::milliseconds(50));
                
                lock_guard<mutex> right_lock(right_fork);
                cout<<"\tPhilosopher "<<id<<" picked up right fork."<<endl;
                
                eat();
            } else {
                lock_guard<mutex> right_lock(right_fork);
                cout<<"\tPhilosopher "<<id<<" picked up right fork."<<endl;
                this_thread::sleep_for(chrono::milliseconds(50));
                
                lock_guard<mutex> left_lock(left_fork);
                cout<<"\tPhilosopher "<<id<<" picked up left fork."<<endl;
                
                eat();
            }
            cout<<"Philosopher "<<id<<" put down both forks."<<endl;
        }
    }
};

int main(){
    int num_philosophers;
    cout<<"Enter the number of philosophers:";
    cin>>num_philosophers;

    if(num_philosophers < 2){
        cout<<"Simulation requires at least 2 philosophers."<<endl;
        return 1;
    }

    cout<<"Dining Philosophers Simulation Started("<<num_philosophers<<" philosophers, manual memory)."<<endl;

    mutex* forks = new mutex[num_philosophers];
    Philosopher** philosophers = new Philosopher*[num_philosophers];
    thread* threads = new thread[num_philosophers];

    for(int i=0;i<num_philosophers;++i){
        philosophers[i] = new Philosopher(i + 1, forks[i], forks[(i + 1) % num_philosophers]);
    }

    for(int i=0;i<num_philosophers;++i){
        threads[i] = thread(&Philosopher::dine, philosophers[i]);
    }

    for(int i=0;i<num_philosophers;++i){
        threads[i].join();
    }

    for(int i=0;i<num_philosophers;++i){
        delete philosophers[i];
    }
    delete[] philosophers;
    delete[] forks;
    delete[] threads;

    cout<<"Dining Philosophers Simulation Ended."<<endl;
    return 0;
}
