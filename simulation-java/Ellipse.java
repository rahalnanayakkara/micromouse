import java.awt.*;
import java.awt.geom.Ellipse2D;

public class Ellipse extends Ellipse2D.Double implements Drawable 
{
	public double centerX, centerY, radiusX, radiusY;
	boolean fill;
	
	public Ellipse(double centerX, double centerY, double radiusX, double radiusY, boolean fill)
	{
		super(centerX-radiusX, centerY-radiusY, 2*radiusX, 2*radiusY);
		this.centerX=centerX;
		this.centerY=centerY;
		this.radiusX=radiusX;
		this.radiusY=radiusY;
		this.fill=fill;
	}
	
	public void setEllipse(double centerX, double centerY, double radiusX, double radiusY)
	{
		super.setFrame(centerX-radiusX, centerY-radiusY, 2*radiusX, 2*radiusY);
		this.centerX=centerX;
		this.centerY=centerY;
		this.radiusX=radiusX;
		this.radiusY=radiusY;
	}
	
	public void draw(Graphics2D g)
	{
		if (fill) g.fill(this);
		else g.draw(this);
	}
}
