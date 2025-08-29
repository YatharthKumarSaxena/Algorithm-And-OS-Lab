#include "linkedlist.cpp"

class Stack {
    LinkedList* list;  
public:
    Stack() {
        list = new LinkedList();   // dynamic allocation
    }

    ~Stack() {
        delete list;   
    }

    void push(char val) {
        list->insertAtHead(val);
    }

    char pop() {
        return list->deleteAtHead();
    }

    char top() {
        return list->peek();
    }

    bool isEmpty() {
        return list->isEmpty();
    }

    int size() {
        return list->size();
    }

    void printStack() {
        cout << "Stack (top -> bottom): ";
        list->printList();
    }
};
