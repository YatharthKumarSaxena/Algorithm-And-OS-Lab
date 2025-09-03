
public class GridDFS {
    static int ROWS, COLS;
    static int[][] grid;
    static boolean[][] visited;

    static MyPoint[] bestPath;
    static MyPoint[] currentPath;
    static int bestProfit = Integer.MIN_VALUE;
    static int bestMoves = Integer.MAX_VALUE;
    static int bestLength = 0;

    // Directions: Up, Down, Left, Right
    static int[] dx = { -1, 1, 0, 0 };
    static int[] dy = { 0, 0, -1, 1 };

    public static void init(int rows, int cols, int[][] g) {
        ROWS = rows;
        COLS = cols;
        grid = g;
        visited = new boolean[ROWS][COLS];
        bestPath = new MyPoint[ROWS * COLS];
        currentPath = new MyPoint[ROWS * COLS];
    }

    static boolean goalReached = false; // New flag

    public static void runDFS() {
        goalReached = false; // Reset
        bestProfit = Integer.MIN_VALUE;
        bestMoves = Integer.MAX_VALUE;
        bestLength = 0;

        dfs(ROWS - 1, COLS - 1, 0, 0, 0);
    }

    private static void dfs(int x, int y, int profit, int moves, int pathLen) {
        if (x < 0 || y < 0 || x >= ROWS || y >= COLS)
            return;
        if (visited[x][y] || grid[x][y] == 9999)
            return;

        profit += grid[x][y];
        moves++;
        visited[x][y] = true;
        currentPath[pathLen] = new MyPoint(x, y);
        pathLen++;

        if (x == 0 && y == 0) {
            goalReached = true;

            boolean allNegative = (bestProfit <= 0 && profit <= 0);

            if (allNegative) {
                // Prefer path with minimum loss (i.e. higher negative profit)
                if (profit > bestProfit || (profit == bestProfit && moves < bestMoves)) {
                    bestProfit = profit;
                    bestMoves = moves;
                    bestLength = pathLen;
                    for (int i = 0; i < pathLen; i++)
                        bestPath[i] = currentPath[i];
                }
            } else {
                // Normal case: maximize profit/move ratio
                double bestRatio = (bestMoves == Integer.MAX_VALUE) ? -1 : (double) bestProfit / bestMoves;
                double currentRatio = (double) profit / moves;

                if (currentRatio > bestRatio ||
                        (currentRatio == bestRatio && profit > bestProfit) ||
                        (currentRatio == bestRatio && profit == bestProfit && moves < bestMoves)) {

                    bestProfit = profit;
                    bestMoves = moves;
                    bestLength = pathLen;
                    for (int i = 0; i < pathLen; i++)
                        bestPath[i] = currentPath[i];
                }
            }

            visited[x][y] = false;
            return;
        }

        if (moves >= ROWS * COLS) {
            visited[x][y] = false;
            return;
        }

        for (int d = 0; d < 4; d++) {
            dfs(x + dx[d], y + dy[d], profit, moves, pathLen);
        }

        visited[x][y] = false;
    }

    public static void printBestPath() {
        if (!goalReached) {
            System.out.println("❌ No path exists from Goal to Start. Grid is fully blocked or unreachable.");
            return;
        }

        double ratio = (bestMoves > 0) ? (double) bestProfit / bestMoves : 0.0;

        System.out.println("✅ Best Path Found with Profit = " + bestProfit +
                ", Moves = " + bestMoves +
                ", Ratio = " + String.format("%.2f", ratio));

        System.out.print("Path (from goal to start): ");
        for (int i = 0; i < bestLength; i++) {
            System.out.print("(" + bestPath[i].x + "," + bestPath[i].y + ") ");
        }
        System.out.println();
    }

    public static MyPoint getPathStep(int step) {
        if (step >= 0 && step < bestLength) {
            return bestPath[step];
        }
        return null;
    }
}
