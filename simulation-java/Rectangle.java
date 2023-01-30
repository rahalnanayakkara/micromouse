import java.awt.Graphics2D;
import java.awt.geom.*;

public class Rectangle extends Rectangle2D.Double implements Drawable
{
	public double x1,y1,x2,y2;
	
	boolean fill;
	
	public Rectangle(double x1, double y1, double x2, double y2,boolean fill)
	{
		super(x1,y1,x2,y2);
		this.fill=fill;
		this.x1=x1;
		this.x2=x2;
		this.y1=y1;
		this.y2=y2;
	}
	
	public Rectangle(Line l, boolean fill)
	{
		this(l.x1,l.y1, l.x2, l.y2,fill);
	}
	
	public void setRect(double x1, double y1, double x2, double y2)
	{
		super.setRect(x1>x2?x2:x1,y1>y2?y2:y1,x1>x2?x1-x2:x2-x1,y1>y2?y1-y2:y2-y1);
		this.x1=x1;
		this.x2=x2;
		this.y1=y1;
		this.y2=y2;
	}
	
	public void setRect(Line l)
	{
		this.setRect(l.x1,l.y1, l.x2, l.y2);
	}
	
	public void draw(Graphics2D g)
	{
		if(fill)g.fill(this);
		else g.draw(this);
	}
	
}
