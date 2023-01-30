import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

public class MazeCanvas extends DrawingCanvas
{
	private static final long serialVersionUID = 1L;

	public static final int EAST = 1, SOUTH = 2, WEST = 3, NORTH = 4;
	
	BufferedImage img;
	File file = new File("sample_maze.JPG");
	int x,y, imgWidth, imgHeight, boxHeight, boxWidth, posX=17, posY=20;
	
	int heading=EAST;
	
	public int left()
	{
		switch (heading)
		{
		case EAST : return NORTH;
		case SOUTH : return EAST;
		case WEST : return SOUTH;
		case NORTH : return WEST;
		}
		return 0;
	}
	
	public int right()
	{
		switch (heading)
		{
		case EAST : return SOUTH;
		case SOUTH : return WEST;
		case WEST : return NORTH;
		case NORTH : return EAST;
		}
		return 0;
	}
	
	public int back()
	{
		switch (heading)
		{
		case EAST : return WEST;
		case SOUTH : return NORTH;
		case WEST : return EAST;
		case NORTH : return SOUTH;
		}
		return 0;
	}
	
	public void loadImage()
	{
		try
		{
			img = ImageIO.read(file);
		}
		catch(IOException e) 
		{
			Screen.show("", "ERROR");
			Screen.show("", ""+file.exists());
		}
		
		imgWidth = img.getWidth();
		imgHeight = img.getHeight();
		
		boxHeight = imgHeight/14;
		boxWidth = imgWidth/14;
		
		//Screen.show("WIDTH", boxWidth);
		//Screen.show("HEIGHT", boxHeight);
	}
	
	public void setX(int x)
	{
		this.x = x;
		posX = 17+35*x;
	}
	
	public void setY(int y)
	{
		this.y = y;
		posY = 20+35*y;
	}
	
	public void draw()
	{
		g.drawImage(img, 1, 1, null);
		circle(posX,posY,5,true);
	}
	
	public boolean isClear(int direction)
	{
		Color c;
		switch (direction)
		{
			case NORTH :for (int i=1; i<35; i++)
						{
							c = new Color(img.getRGB(posX, posY-i));
							if (c.getBlue()<200) return false;
						} break;
			case SOUTH :for (int i=1; i<35; i++)
						{
							c = new Color(img.getRGB(posX, posY+i));
							if (c.getBlue()<200) return false;
						} break;
			case EAST :for (int i=1; i<35; i++)
						{
							c = new Color(img.getRGB(posX+i, posY));
							if (c.getBlue()<200) return false;
						} break;
			case WEST :for (int i=1; i<35; i++)
						{
							c = new Color(img.getRGB(posX-i, posY));
							if (c.getBlue()<200) return false;
						} break;
		}
		return true;
	}
	
	public void move()
	{
		switch (heading)
		{
			case NORTH : posY-=35; y--; break;
			case SOUTH : posY+=35; y++; break;
			case EAST : posX+=35; x++; break;
			case WEST : posX-=35; x--; break;
		}
		repaint();
	}
	
	public void leftFirst()
	{
		if (isClear(left())) heading = left();
		else if (isClear(heading));
		else if (isClear(right())) heading = right();
		else heading = back();
		move();
	}
	
	public void rightFirst()
	{
		if (isClear(right())) heading = right();
		else if (isClear(heading));
		else if (isClear(left())) heading = left();
		else heading = back();
		move();
	}
	
	public void test()
	{
		while (true)
		{
			delay(500);
			rightFirst();
		}
	}
	
	public void delay(int time)
	{
		try {
			Thread.sleep(time);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static void main(String[] args) 
	{
		MazeCanvas tc;
		GridForm f = new GridForm("Test",10,10,45,80,20,20);
		f.add(tc = new MazeCanvas(),1,1,500/f.colWidth+1,500/f.rowHeight+1);
		tc.loadImage();
		tc.setupCoordinateSystem(0, 500, 0, 500);
		tc.draw();
		tc.repaint();
		tc.test();
	}

}
