#include "stack.cpp"
using namespace std;

void printAllStacks(Stack &s1, Stack &s2, Stack &s3, string label) {
    cout << "\n---- " << label << " ----\n";
    cout << "S1: "; s1.printStack();
    cout << "\nS2: "; s2.printStack();
    cout << "\nS3: "; s3.printStack();
    cout << "\n-------------------------\n";
}

void sortStacks(int& count, string str, Stack &s1, Stack &s2, Stack &s3) {
    char pivot = 'A';   // start pivot

    while (!s1.isEmpty()) {
        
        bool pivotNotExist = true;
        
        for(int i=0;i<str.length();i++){
            if(str[i] == pivot){
                pivotNotExist = false;
                break;
            }
        }

        if(pivotNotExist){
            pivot++;
            continue;
        }

        cout << "\nProcessing pivot: " << pivot << endl;

        // Step 1: process all elements of s1 wrt pivot
        while (!s1.isEmpty()) {
            char ch = s1.pop();
            if (ch == pivot) {
                cout << "Popped " << ch << " from S1 -> Pushed to S2\n";
                s2.push(ch);  // correct place
            } else {
                cout << "Popped " << ch << " from S1 -> Pushed to S3\n";
                s3.push(ch);  // wrong place
            }
            count++;
        }
        printAllStacks(s1, s2, s3, "After distributing wrt pivot");

        // Step 2: move s3 back into s1 for next iteration
        if (!s3.isEmpty()) {
            cout << "Moving S3 back to S1...\n";
            while (!s3.isEmpty()) {
                s1.push(s3.pop());
                count++;
            }
        } else {
            cout << "S3 empty -> Sorting complete till pivot " << pivot << "\n";
            break;
        }
        printAllStacks(s1, s2, s3, "After restoring S1 from S3");

        // Step 3: move to next pivot
        pivot++;
    }

    // Step 4: transfer result from s2 to s1 (sorted order)
    cout << "\nTransferring final sorted from S2 -> S1...\n";
    while (!s2.isEmpty()) {
        s1.push(s2.pop());
        count++;
    }
    printAllStacks(s1, s2, s3, "Final sorted stacks");
}

int main() {
    string str = "XASCDA";

    Stack s1, s2, s3;  // s1 = input, s2 = sorted, s3 = temp

    int count = 0;

    // Push input into s1 in reverse order (so first char is on top)
    for (int i = str.size() - 1; i >= 0; i--) {
        s1.push(str[i]);
    }

    cout << "Initial Stack (s1): ";
    s1.printStack();

    sortStacks(count, str, s1, s2, s3);

    cout << "\nFinal Sorted Output (s1): ";
    s1.printStack();

    cout<<"Total Steps:- "<<count<<endl;

    return 0;
}
