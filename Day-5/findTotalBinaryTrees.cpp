#include <iostream>
using namespace std;

// Function to calculate 2^n - 1 limit
int calculateLimit(int nodes){
    int limit = 1;
    for(int i=1; i<=nodes; i++){
        limit *= 2;
    }
    return limit-1;
}

// Left and right limits ke saath heap-index recursion
int findTotalBinaryTrees(int leftLimit, int rightLimit, int count, int nodes, int totalLimit){
    // Base cases
    if(nodes == 1) return 1;           // Single node
    if(count == nodes) return 1;       // All nodes placed
    if(leftLimit > totalLimit && rightLimit > totalLimit) return 0;  // Both limits exceed

    // Count possibilities for left subtree
    int leftWays = 0;
    if(leftLimit <= totalLimit){
        leftWays = findTotalBinaryTrees(leftLimit*2, rightLimit, count + 1, nodes, totalLimit);
    }

    // Count possibilities for right subtree
    int rightWays = 0;
    if(rightLimit <= totalLimit){
        rightWays = findTotalBinaryTrees(leftLimit, rightLimit*2 + 1, count + 1, nodes, totalLimit);
    }

    int bothWays = 0;
    if(count + 2 <= nodes){
        bothWays = findTotalBinaryTrees(leftLimit*2, rightLimit*2 + 1, count + 2, nodes, totalLimit);
    }
    return leftWays + rightWays + bothWays;  // If only one side exists
}

int main(){
    int n;
    cout<<"Welcome to the world of Programming\n";
    cin >> n;
    if(n <= 0){
        cout << "Enter Positive Number Of Nodes\n";
        return 0;
    }

    int limit = calculateLimit(n);  // 2^n - 1
    cout << "Total Binary Trees possible are:- " 
         << findTotalBinaryTrees(1, 1, 1, n, limit) << endl;

    return 0;
}
