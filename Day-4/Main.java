import javax.swing.*;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) throws Exception {
        Scanner sc = new Scanner(System.in);

        System.out.print("Enter rows: ");
        int rows = sc.nextInt();
        System.out.print("Enter cols: ");
        int cols = sc.nextInt();

        int[][] grid = new int[rows][cols];
        System.out.println("Enter grid values (positive = gift, negative = pothole, 9999 = blocker):");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid[i][j] = sc.nextInt();
            }
        }

        // Initialize and run DFS
        GridDFS.init(rows, cols, grid);
        GridDFS.runDFS();

        if (!GridDFS.pathFound || GridDFS.bestLength == 0) {
            System.out.println("❌ No path exists from Goal to Start. Grid is fully blocked or unreachable.");
        } else {
            GridDFS.printBestPath();

            System.out.println("\n✅ Best Path Found with Profit = " + GridDFS.bestProfit +
                    ", Moves = " + GridDFS.bestMoves +
                    ", Ratio = " + ((double) GridDFS.bestProfit / GridDFS.bestMoves));

            System.out.print("Path (from start to end): ");
            for (int i = 0; i < GridDFS.bestLength; i++) {
                System.out.print("(" + GridDFS.bestPath[i].x + "," + GridDFS.bestPath[i].y + ")");
                if (i < GridDFS.bestLength - 1)
                    System.out.print(" -> ");
            }
            System.out.println();
        }

        // GUI
        JFrame frame = new JFrame("Grid Path Visualization");
        GridPanel panel = new GridPanel();
        frame.add(panel);
        frame.setSize(cols * 80, rows * 80);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);

        // Animate
        for (int step = 0; step < GridDFS.bestLength; step++) {
            panel.setStep(step);
            Thread.sleep(700);
        }
    }
}
