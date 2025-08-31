import javax.swing.*;
import java.awt.*;

public class GridPanel extends JPanel {
    int step = 0;
    int cellSize = 60;

    public void setStep(int s) {
        step = s;
        repaint();
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);

        for (int i = 0; i < GridDFS.ROWS; i++) {
            for (int j = 0; j < GridDFS.COLS; j++) {
                int val = GridDFS.grid[i][j];

                if (val == 9999) {
                    g.setColor(Color.BLACK);
                    g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                } else if (val > 0) {
                    g.setColor(Color.GREEN);
                    g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                } else if (val < 0) {
                    g.setColor(Color.RED);
                    g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                } else {
                    g.setColor(Color.LIGHT_GRAY);
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

        // GridPanel.java ke andar
        if (step < GridDFS.bestLength) {
            java.awt.Point p = GridDFS.getPathStep(step); // 👈 force java.awt.Point
            g.setColor(Color.BLUE);
            g.fillOval(p.y * cellSize + 15, p.x * cellSize + 15, 30, 30);
        }

    }
}
