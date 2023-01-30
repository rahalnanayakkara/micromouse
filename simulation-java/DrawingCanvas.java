import java.awt.*;

import java.awt.geom.*;

public class DrawingCanvas extends Canvas 
{
	private static final long serialVersionUID = 1L;
	public Graphics2D g;
	protected Graphics2D gText;
	FontMetrics fm;
	protected double xScale, yScale, xTranslate, yTranslate, left, right, top, bottom;
	Color color;
	
	public void setupCoordinateSystem(double left, double right, double top, double bottom)
	{
		this.left=left;
		this.right=right;
		this.top=top;
		this.bottom=bottom;
		setupCoordinateSystem(getGraphics());
	}
	
	public void setupCoordinateSystem(Graphics graphics)
	{
		g = (Graphics2D)graphics;
		gText = (Graphics2D)g.create();
		fm = g.getFontMetrics();
		xScale = getWidth()/(right-left);
		yScale = getHeight()/(bottom-top);
		xTranslate = -left;
		yTranslate = -top;
		g.setStroke(new BasicStroke(0));
		g.scale(xScale, yScale);
		g.translate(-left, -top);
	}
	
	public void clear()
	{
		g.setColor(getBackground());
		box(left,bottom,right,top,true);
		g.setColor(color);
	}
	
	
	public void paint(Graphics g)
	{
		setupCoordinateSystem(g);
		draw();
	}
	
	public void line(double x1, double y1, double x2, double y2)
	{
		new Line(x1,y1,x2,y2).draw(g);
	}
	
	public void box(double x1, double y1, double x2, double y2, boolean fill)
	{
		new Rectangle(x1,y1,x2,y2,fill).draw(g);
		
	}
	
	public void ellipse(double centerX, double centerY, double radiusX, double radiusY, boolean fill)
	{
		new Ellipse(centerX, centerY, radiusX, radiusY,fill).draw(g);
	}
	
	public void circle(double centerX, double centerY, double radius, boolean fill)
	{
		new Ellipse(centerX, centerY, radius, radius,fill).draw(g);
	}
	
	public void text(String s, double x, double y, double xAlign, double yAlign)
	{
		x = (x+xTranslate)*xScale;
		y = (y+yTranslate)*yScale;
		x -= xAlign*fm.stringWidth(s);
		y +=yAlign*fm.getMaxAscent();
		gText.drawString(s, (float)x, (float)y);
	}
	
	public void text(String s, double x, double y)
	{
		text(s,x,y,0,0);
	}
	
	public void setColor(Color color)
	{
		g.setColor(color);
		gText.setColor(color);
		this.color = color;
	}
	
	public void setColor(int red, int green, int blue)
	{
		setColor(new Color(red,green,blue));
	}
	
	public void setFont(Font font)
	{
		gText.setFont(font);
		fm = gText.getFontMetrics();
	}
	
	public void setFont(String name, int style, int size)
	{
		setFont(new Font(name, style, size));
	}
	
	public void setLineWidth(double lineWidth)
	{
		g.setStroke(new BasicStroke((float)lineWidth));
	}
	
	public void setDashedLine(double dashLength, double lineWidth)
	{
		float[] dash = new float[2];
		dash[0] = dash[1] = (float)dashLength;
		g.setStroke(new BasicStroke((float)lineWidth,BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL,0,dash,0));
	}
	
	public void draw(){}
	
	public static final double LEFT = 0;
	public static final double CENTER = 0.5;
	public static final double RIGHT = 1;
	public static final double BOTTOM = 0;
	public static final double TOP = 1;
}
