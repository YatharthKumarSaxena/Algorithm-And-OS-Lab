#include <iostream>
using namespace std;

// Node class
class ListNode {
public:
    char val;
    ListNode* next;

    ListNode(char val) {
        this->val = val;
        this->next = NULL;
    }
};

// Singly Linked List class
class LinkedList {
private:
    ListNode* head;
    int length;

public:
    // Constructor
    LinkedList() {
        this->head = NULL;
        this->length = 0;
    }

    // Insert at head
    void insertAtHead(char val) {
        ListNode* newNode = new ListNode(val);
        newNode->next = head;
        head = newNode;
        length++;
    }

    // Delete at head
    char deleteAtHead() {
        if (isEmpty()) {
            cout << "List is empty, cannot delete!\n";
            return '\0';
        }
        ListNode* temp = head;
        char deleteChar = temp->val;
        head = head->next;
        delete temp;
        length--;
        return deleteChar;
    }

    // Check if empty
    bool isEmpty() {
        return head == NULL;
    }

    // Size of list
    int size() {
        return length;
    }

    // Peek at head value
    char peek() {
        if (isEmpty()) {
            cout << "List is empty!\n";
            return '\0'; // null char
        }
        return head->val;
    }

    // Print list (for debugging)
    void printList() {
        ListNode* temp = head;
        cout << "List: ";
        while (temp != NULL) {
            cout << temp->val << " -> ";
            temp = temp->next;
        }
        cout << "NULL\n";
    }
};