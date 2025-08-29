import java.util.Scanner;

public class Main {
    static final int MAX_NODES = 1000;
    static int[] parentNode = new int[MAX_NODES];
    static int[] setSize = new int[MAX_NODES];

    static void createSet(int node) {
        parentNode[node] = node;
        setSize[node] = 1;
    }

    static int findParent(int node) {
        if (node == parentNode[node]) {
            return node;
        }
        return parentNode[node] = findParent(parentNode[node]);
    }

    static void mergeSets(int a, int b) {
        a = findParent(a);
        b = findParent(b);
        if (a != b) {
            if (setSize[a] < setSize[b]) {
                int temp = a;
                a = b;
                b = temp;
            }
            parentNode[b] = a;
            setSize[a] += setSize[b];
        }
    }

    static int calculateMST(int[] socketType, int[][] connections, int numSockets, int numEdges) {
        for (int i = 0; i < numSockets; i++) {
            createSet(i);
        }

        int[][] validEdges = new int[numEdges][3];
        int validEdgeCount = 0;

        for (int i = 0; i < numEdges; i++) {
            int u = connections[i][1];
            int v = connections[i][2];
            if (socketType[u] != 0 && socketType[v] != 0 && u != v) {
                for (int j = 0; j < 3; j++) {
                    validEdges[validEdgeCount][j] = connections[i][j];
                }
                validEdgeCount++;
            }
        }

        // Bubble sort validEdges based on weight (column 0)
        for (int i = 0; i < validEdgeCount - 1; i++) {
            for (int j = 0; j < validEdgeCount - i - 1; j++) {
                if (validEdges[j][0] > validEdges[j + 1][0]) {
                    // Swap the entire row
                    int[] temp = validEdges[j];
                    validEdges[j] = validEdges[j + 1];
                    validEdges[j + 1] = temp;
                }
            }
        }

        int requiredConnections = 0;
        for (int i = 0; i < numSockets; i++) {
            if (socketType[i] != 0) {
                requiredConnections++;
            }
        }
        if (requiredConnections <= 1) {
            return 0;
        }

        int totalCost = 0;
        int connectedEdges = 0;
        System.out.println("MST formed as given below:- ");
        for (int i = 0; i < validEdgeCount; i++) {
            int u = validEdges[i][1];
            int v = validEdges[i][2];
            int w = validEdges[i][0];
            int parentU = findParent(u);
            int parentV = findParent(v);
            if (parentU != parentV) {
                totalCost += w;
                connectedEdges++;
                System.out.println("Edge :- "+u+"-"+v+": "+w);
                mergeSets(u, v);
                if (connectedEdges == requiredConnections - 1) {
                    return totalCost;
                }
            }
        }
        return -1;
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        System.out.print("Enter number of sockets: ");
        int numSockets = sc.nextInt();

        if(numSockets == 1){
            System.out.println("Only One Socket so no joining possible. So Total wire length = 0");
            return;
        }
        int[] ground = new int[numSockets];
        int[] neutral = new int[numSockets];
        int[] live = new int[numSockets];

        System.out.println("Enter points for socket (G N L) in 0 and 1 form: ");

        for (int i = 0; i < numSockets; i++) {
            ground[i] = sc.nextInt();
            neutral[i] = sc.nextInt();
            live[i] = sc.nextInt();
        }

        int maxEdges = (numSockets * (numSockets - 1)) / 2;
        int actualEdges = 0;
        int[][] edges = new int[maxEdges][3];

        System.out.println("Enter source socket, destination socket and connection distance");

        while (true) {

            edges[actualEdges][1] = sc.nextInt(); // source
            edges[actualEdges][2] = sc.nextInt(); // destination
            edges[actualEdges][0] = sc.nextInt();

            actualEdges++;
            System.out.print("Do you want to add another edge (Y/N)? ");
            char choice = sc.next().charAt(0);
            if (choice != 'Y' && choice != 'y') {
                break;
            }
        }

        int groundCost = calculateMST(ground, edges, numSockets, actualEdges);
        int neutralCost = calculateMST(neutral, edges, numSockets, actualEdges);
        int liveCost = calculateMST(live, edges, numSockets, actualEdges);

        if (groundCost < 0 || neutralCost < 0 || liveCost < 0) {
            if (groundCost < 0)
                System.out.println("Impossible to connect all Ground points.");
            if (neutralCost < 0)
                System.out.println("Impossible to connect all Neutral points.");
            if (liveCost < 0)
                System.out.println("Impossible to connect all Live points.");
        } else {
            System.out.println("Minimum wire length for Ground: " + groundCost);
            System.out.println("Minimum wire length for Neutral: " + neutralCost);
            System.out.println("Minimum wire length for Live: " + liveCost);
            System.out.println("Total wire length needed: " + (groundCost + neutralCost + liveCost));
        }

        sc.close();
    }
}
