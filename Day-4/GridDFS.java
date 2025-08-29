import javax.swing.*;
import java.awt.*;
import java.util.Scanner;

class Point {
    int x, y;
    Point(int x, int y) { this.x = x; this.y = y; }
}

public class GridDFS extends JPanel {
    static int ROWS, COLS;
    static int[][] grid;
    static boolean[][] visited;

    static Point[] bestPath;
    static Point[] currentPath;
    static int bestProfit = Integer.MIN_VALUE;
    static int bestMoves = Integer.MAX_VALUE;
    static int bestLength = 0;

    // Directions: Up, Down, Left, Right
    static int[] dx = {-1, 1, 0, 0};
    static int[] dy = {0, 0, -1, 1};

    // For animation
    static int step = 0;

    public static void main(String[] args) throws Exception {
        Scanner sc = new Scanner(System.in);

        System.out.print("Enter rows: ");
        ROWS = sc.nextInt();
        System.out.print("Enter cols: ");
        COLS = sc.nextInt();

        grid = new int[ROWS][COLS];
        visited = new boolean[ROWS][COLS];
        bestPath = new Point[ROWS * COLS];
        currentPath = new Point[ROWS * COLS];

        System.out.println("Enter grid values (positive = gift, negative = pothole, 9999 = blocker):");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                grid[i][j] = sc.nextInt();
            }
        }

        // Run DFS
        dfs(ROWS - 1, COLS - 1, 0, 0, 0);

        System.out.println("\n✅ Best Path Found with Profit = " + bestProfit + ", Moves = " + bestMoves +
                ", Ratio = " + ((double)bestProfit / bestMoves));

        // Print path in terminal
        System.out.print("Path (from start to end): ");
        for (int i = 0; i < bestLength; i++) {
            System.out.print("(" + bestPath[i].x + "," + bestPath[i].y + ")");
            if (i < bestLength - 1) System.out.print(" -> ");
        }
        System.out.println();

        // Swing window
        JFrame frame = new JFrame("Grid Path Visualization");
        GridDFS panel = new GridDFS();
        frame.add(panel);
        frame.setSize(COLS * 80, ROWS * 80);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);

        // Animate path
        for (step = 0; step < bestLength; step++) {
            panel.repaint();
            Thread.sleep(700);
        }
    }

    static void dfs(int x, int y, int profit, int moves, int pathLen) {
        if (x < 0 || y < 0 || x >= ROWS || y >= COLS || visited[x][y] || grid[x][y] == 9999)
            return;

        profit += grid[x][y];
        moves++;
        visited[x][y] = true;
        currentPath[pathLen] = new Point(x, y);
        pathLen++;

        if (x == 0 && y == 0) {
            double bestRatio = bestMoves == 0 ? -1 : (double)bestProfit / bestMoves;
            double currentRatio = (double)profit / moves;

            if (currentRatio > bestRatio) {
                bestProfit = profit;
                bestMoves = moves;
                bestLength = pathLen;
                for (int i = 0; i < pathLen; i++) bestPath[i] = currentPath[i];
            }
        }

        for (int d = 0; d < 4; d++) {
            dfs(x + dx[d], y + dy[d], profit, moves, pathLen);
        }

        visited[x][y] = false;
    }

    // Draw the grid
    public void paintComponent(Graphics g) {
        super.paintComponent(g);

        int cellSize = 60;

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int val = grid[i][j];

                if (val == 9999) {
                    g.setColor(Color.BLACK); // Blocker
                    g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                } else if (val > 0) {
                    g.setColor(Color.GREEN); // Gifts
                    g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                } else if (val < 0) {
                    g.setColor(Color.RED); // Potholes
                    g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                } else {
                    g.setColor(Color.LIGHT_GRAY); // Empty
                    g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                }

                g.setColor(Color.BLACK);
                g.drawRect(j * cellSize, i * cellSize, cellSize, cellSize);

                if (val != 9999 && val != 0) {
                    g.setColor(Color.BLACK);
                    g.drawString(String.valueOf(val), j * cellSize + 20, i * cellSize + 30);
                }
            }
        }

        // Draw agent (animated)
        if (step < bestLength) {
            Point p = bestPath[step];
            g.setColor(Color.BLUE);
            g.fillOval(p.y * cellSize + 15, p.x * cellSize + 15, 30, 30);
        }
    }
}
