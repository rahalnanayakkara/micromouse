import java.awt.geom.Line2D;
import java.awt.Graphics2D;

public class Line extends Line2D.Double implements Drawable
{
	public Line(double x1, double y1, double x2, double y2)
	{
		super(x1, y1, x2, y2);
	}
	
	public void draw(Graphics2D g)
	{
		g.draw(this);
	}

}
