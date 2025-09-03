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
                g.setColor(Color.BLACK); // blocker
            } else if (val > 0) {
                g.setColor(Color.GREEN); // gift
            } else if (val < 0) {
                g.setColor(Color.RED); // pothole
            } else {
                g.setColor(Color.LIGHT_GRAY); // normal cell
            }
            g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);

            // Highlight path up to current step
            if (isInCurrentPath(i, j)) {
                g.setColor(new Color(255, 255, 0, 150)); // transparent yellow
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

    // Current agent position (blue circle)
    if (step < GridDFS.bestLength) {
        MyPoint p = GridDFS.getPathStep(step);
        g.setColor(Color.BLUE);
        g.fillOval(p.y * cellSize + 15, p.x * cellSize + 15, 30, 30);
    }
}

// ✅ Helper method (sirf current step tak ke path ko check karega)
private boolean isInCurrentPath(int i, int j) {
    for (int k = 0; k <= step && k < GridDFS.bestLength; k++) {
        MyPoint p = GridDFS.bestPath[k];
        if (p.x == i && p.y == j) {
            return true;
        }
    }
    return false;
}

}
