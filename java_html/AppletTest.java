/**
 * @(#)AppletTest.java
 *
 * Sample Applet application
 *
 * @author 
 * @version 1.00 06/05/23
 */
 
import java.awt.*;
import javax.swing.*;

public class AppletTest extends JApplet 
{
	
	public void init() 
	{
		
	}

	public void paint(Graphics g) 
	{
		g.drawRect(100, 100, 200, 199);
		g.setColor(Color.yellow);
		g.fillRect(101, 101, 199, 198);
		
		g.setColor(Color.black);
		int xPoints[] = new int[3];
		xPoints[0] = 100;
		xPoints[1] = 200;
		xPoints[2] = 300;
		int yPoints[] = new int[3];
		yPoints[0] = 100;
		yPoints[1] = 0;
		yPoints[2] = 100;
		int nPoints = 3;		
		g.fillPolygon(xPoints, yPoints, nPoints);
		
		g.setColor(Color.black);
		g.drawRect(230, 220, 50, 79);
		g.setColor(Color.red);
		g.fillRect(231, 221, 49, 78);
		
		g.setColor(Color.white);
		g.fillOval(270, 260, 3, 3);
		
		g.setColor(Color.cyan);
		g.fillOval(105, 110, 195, 80);
		
		g.setColor(Color.green);
		g.fillOval(110, 205, 90, 90);
	}
}