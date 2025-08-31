// Submitted By:- Yatharth Kumar Saxena
// Enrollment Number:- GM7058
// Faculty Number:- 23COBEA199

#include <iostream>
#include <string>
#include <climits>
using namespace std;

static const int MAX = 26;
int normalGraph[MAX][MAX];
int backboneGraph[MAX][MAX];
bool visitedArr[MAX];
string bestPath = "";
int maxBackboneCount = -1;
string currentPath = "";
int bestTotalEdges = INT_MAX; // prefer fewer edges on tie

void dfs(int current, int dest, int backboneCount, int totalEdges){
    if(current == dest){
        // tie-breaker: prefer larger backboneCount, if equal prefer smaller totalEdges
        if (backboneCount > maxBackboneCount ||
           (backboneCount == maxBackboneCount && totalEdges < bestTotalEdges)){
            maxBackboneCount = backboneCount;
            bestPath = currentPath + char('A' + dest);
            bestTotalEdges = totalEdges;
        }
        return;
    }

    visitedArr[current] = true;
    currentPath += char('A' + current);
    currentPath += " -> ";

    for(int i = 0; i < MAX; ++i){
        if((normalGraph[current][i] == 1 || backboneGraph[current][i] == 1) && !visitedArr[i]){
            int added = backboneGraph[current][i]; // 1 if backbone edge, else 0
            dfs(i, dest, backboneCount + added, totalEdges + 1);
        }
    }

    visitedArr[current] = false;
    if (currentPath.size() >= 5) currentPath.erase(currentPath.size() - 5);
}

int main(){
    cout<<"First Enter number of Edges in First Line\n";
    cout<<"Enter each edge as: <Node1> <Node2> <type: normal/backbone>\n";
    cout<<"Then Enter Source Node in New Line\n";
    cout<<"Then Enter Destination Node in Last Line\n";
    
    for(int i = 0; i < MAX; ++i){
        for (int j = 0; j < MAX; ++j){
            normalGraph[i][j] = 0;
            backboneGraph[i][j] = 0;
        }
    }
    
    for(int i = 0; i < MAX; ++i) visitedArr[i] = false;

    int totalEdges;
    cin>>totalEdges;

    for(int i = 0; i < totalEdges; ++i){
        char u, v;
        string type;
        cin>>u>>v>>type;
        if (u >= 'a' && u <= 'z') u = char(u - 'a' + 'A');
        if (v >= 'a' && v <= 'z') v = char(v - 'a' + 'A');
        int a = u - 'A';
        int b = v - 'A';
        if(a < 0 || a >= MAX || b < 0 || b >= MAX){
            cout<<"Node must be A-Z. Skipping invalid edge.\n";
            continue;
        }
        if(type == "backbone"){
            backboneGraph[a][b] = 1;
            backboneGraph[b][a] = 1;
        }
        normalGraph[a][b] = 1;
        normalGraph[b][a] = 1;
    }

    char srcChar;
    cin>>srcChar;
    if(srcChar >= 'a' && srcChar <= 'z')srcChar = char(srcChar - 'a' + 'A');
    char destChar;
    cin>>destChar;
    if(destChar >= 'a' && destChar <= 'z')destChar = char(destChar - 'a' + 'A');

    int src = srcChar - 'A';
    int dest = destChar - 'A';

    if(src < 0 || src >= MAX || dest < 0 || dest >= MAX){
        cout<<"Source or destination must be A-Z.\n";
        return 0;
    }

    // same node
    if(src == dest){
        cout<<"Source is same as destination: "<<srcChar<<endl;
        cout<<"Backbone edges in path: 0\n";
        cout<<"Total streets (edges) in path: 0\n";
        return 0;
    }

    // Direct connection check
    if(normalGraph[src][dest] == 1 || backboneGraph[src][dest] == 1){
        int directBackbone = backboneGraph[src][dest];
        cout<<"Direct path: "<<srcChar<< " -> "<<destChar<<endl;
        cout<<"Backbone edges in path: "<<directBackbone<<endl;
        cout<<"Total streets (edges) in path: 1\n";
        return 0;
    }

    // DFS to find path with maximum backbone edges (with tie-breaker)
    dfs(src, dest, 0, 0);

    if(maxBackboneCount == -1){
        cout<<"No path found.\n";
    }else{
        cout<<"Best path with most backbone edges ("<<maxBackboneCount<<"): "<<bestPath<<endl;
        cout<<"Total backbone edges in this path: "<<maxBackboneCount<<endl;
        cout<<"Total streets (edges) in this path: "<<bestTotalEdges<<endl;
    }

    return 0;
}