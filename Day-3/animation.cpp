#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "stack.cpp"

using namespace std;

string letterColors[26]; // 0 = 'A', 1 = 'B', ..., 25 = 'Z'

// stack colors
const string COLOR_ONE   = "\033[38;5;39m";   // blue-ish
const string COLOR_TWO   = "\033[38;5;46m";   // green-ish
const string COLOR_THREE = "\033[38;5;208m";  // orange-ish

// ANSI helpers
const string RESET_FMT = "\033[0m";
const string BOLD_FMT  = "\033[1m";
const string DIM_FMT   = "\033[2m";
const string CLEAR_SCR = "\033[2J\033[H";

// initialize letter colors
void setupLetterColors() {
    letterColors[0]  = "\033[38;5;51m";   // A
    letterColors[1]  = "\033[38;5;27m";   // B
    letterColors[2]  = "\033[38;5;51m";   // C
    letterColors[3]  = "\033[38;5;208m";  // D
    letterColors[4]  = "\033[38;5;118m";  // E
    letterColors[5]  = "\033[38;5;196m";  // F
    letterColors[6]  = "\033[38;5;34m";   // G
    letterColors[7]  = "\033[38;5;202m";  // H
    letterColors[8]  = "\033[38;5;54m";   // I
    letterColors[9]  = "\033[38;5;33m";   // J
    letterColors[10] = "\033[38;5;129m";  // K
    letterColors[11] = "\033[38;5;190m";  // L
    letterColors[12] = "\033[38;5;201m";  // M
    letterColors[13] = "\033[38;5;27m";   // N
    letterColors[14] = "\033[38;5;208m";  // O
    letterColors[15] = "\033[38;5;219m";  // P
    letterColors[16] = "\033[38;5;93m";   // Q
    letterColors[17] = "\033[38;5;196m";  // R
    letterColors[18] = "\033[38;5;45m";   // S
    letterColors[19] = "\033[38;5;40m";   // T
    letterColors[20] = "\033[38;5;135m";  // U
    letterColors[21] = "\033[38;5;171m";  // V
    letterColors[22] = "\033[38;5;220m";  // W
    letterColors[23] = "\033[38;5;27m";   // X
    letterColors[24] = "\033[38;5;226m";  // Y
    letterColors[25] = "\033[38;5;202m";  // Z
}

// get color for letter
string getLetterColor(char ch) {
    if (ch >= 'A' && ch <= 'Z') return letterColors[ch - 'A'];
    return "\033[97m"; // white default
}

// sleep utility
void pauseMS(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

// render one stack for printing
string renderStack(const char items[], int count, const string& labelColor, const string& label) {
    string out = labelColor + BOLD_FMT + label + RESET_FMT + "  ";
    out += DIM_FMT + "[top]" + RESET_FMT + " ";
    for (int i = 0; i < count; i++) {
        out += getLetterColor(items[i]) + string(1, items[i]) + RESET_FMT + " ";
    }
    out += DIM_FMT + "[bottom]" + RESET_FMT;
    return out;
}

// take snapshot of stack
int grabSnapshot(Stack &stk, char buffer[]) {
    int n = 0;
    Stack tmp;
    while (!stk.isEmpty()) {
        char ch = stk.pop();
        buffer[n++] = ch;
        tmp.push(ch);
    }
    while (!tmp.isEmpty()) stk.push(tmp.pop());
    return n;
}

void displayStacks(Stack &stk1, Stack &stk2, Stack &stk3, const string& msg) {
    char arr1[200], arr2[200], arr3[200];
    int len1 = grabSnapshot(stk1, arr1);
    int len2 = grabSnapshot(stk2, arr2);
    int len3 = grabSnapshot(stk3, arr3);

    cout << CLEAR_SCR;
    cout << BOLD_FMT << "3-Stack Demo" << RESET_FMT << "  "
         << DIM_FMT << msg << RESET_FMT << "\n\n";

    cout << renderStack(arr1, len1, COLOR_ONE, "Stk1") << "\n";
    cout << renderStack(arr2, len2, COLOR_TWO, "Stk2") << "\n";
    cout << renderStack(arr3, len3, COLOR_THREE, "Stk3") << "\n\n";

    // Dynamic legend logic
    char legend[26]; // max 26 unique uppercase letters
    int legendLen = 0;

    auto addToLegend = [&](char ch) {
        if (ch < 'A' || ch > 'Z') return;
        for (int i = 0; i < legendLen; ++i) {
            if (legend[i] == ch) return;
        }
        legend[legendLen++] = ch;
    };

    for (int i = 0; i < len1; ++i) addToLegend(arr1[i]);
    for (int i = 0; i < len2; ++i) addToLegend(arr2[i]);
    for (int i = 0; i < len3; ++i) addToLegend(arr3[i]);

    cout << DIM_FMT << "Legend: " << RESET_FMT;
    for (int i = 0; i < legendLen; ++i) {
        cout << getLetterColor(legend[i]) << legend[i] << RESET_FMT << " ";
    }
    cout << "\n";
    cout.flush();
}


// move item with animation
void transferWithAnim(int& count, Stack &src, Stack &dst, const string& srcLabel, const string& dstLabel,Stack &stk1, Stack &stk2, Stack &stk3) {
    if (src.isEmpty()) return;
    char ch = src.pop();
    dst.push(ch);
    count++;
    string action = "Move " + string(1, ch) + "  " + srcLabel + " -> " + dstLabel;
    displayStacks(stk1, stk2, stk3, action);
    pauseMS(250);
}

// iterative sorting using 3 stacks
void sortThreeStacks(int& count, string str, Stack &stk1, Stack &stk2, Stack &stk3) {
    char pivot = 'A';
    displayStacks(stk1, stk2, stk3, "Start");

    while (!stk1.isEmpty()) {
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

        while (!stk1.isEmpty()) {
            char topCh = stk1.top();
            if (topCh == pivot) {
                transferWithAnim(count, stk1, stk2, "Stk1", "Stk2", stk1, stk2, stk3);
            } else {
                transferWithAnim(count, stk1, stk3, "Stk1", "Stk3", stk1, stk2, stk3);
            }
        }
        displayStacks(stk1, stk2, stk3, string("Distributed for pivot '") + pivot + "'");
        if (stk3.isEmpty()) break;
        while (!stk3.isEmpty()) transferWithAnim(count, stk3, stk1, "Stk3", "Stk1", stk1, stk2, stk3);
        displayStacks(stk1, stk2, stk3, "Restored Stk3 -> Stk1");
        pivot++;
        pauseMS(100);
    }

    while (!stk2.isEmpty()) transferWithAnim(count, stk2, stk1, "Stk2", "Stk1", stk1, stk2, stk3);
    displayStacks(stk1, stk2, stk3, "Done");
}

int main() {
    setupLetterColors();
    string text = "AABCBBBCC";
    Stack first, second, third;

    int count = 0;

    for (int i = (int)text.size() - 1; i >= 0; --i) first.push(text[i]);

    sortThreeStacks(count, text, first, second, third);

    cout << "\nFinal Sorted (top->bottom): ";
    first.printStack();
    cout << "\n";

    cout<<"Total Steps:- "<<count<<endl;

    return 0;
}
