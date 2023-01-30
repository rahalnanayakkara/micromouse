
import java.awt.Color;
import java.util.Stack;

public class MemoryMaze extends MazeCanvas 
{
	Cell[][] cell = new Cell[14][14];
	Cell currCell;
	boolean returning;
	boolean finished;
	Stack<Cell> visitedCells = new Stack<Cell>();
	public MemoryMaze()
	{
		super();
		for (int x=0; x<14; x++) for (int y=0; y<14; y++)
			{
				cell[x][y] = new Cell();
				cell[x][y].x=x;
				cell[x][y].y=y;
			}
		for (int y=0; y<14; y++) cell[0][y].wall[WEST]=true;
		for (int y=0; y<14; y++) cell[13][y].wall[EAST]=true;
		for (int x=0; x<14; x++) cell[x][13].wall[SOUTH]=true;
		for (int x=0; x<14; x++) cell[x][0].wall[NORTH]=true;
		x=13;y=0;
		posX = 17+13*35;
		heading = WEST;
	}
	
	public void wallCheck()
	{
		currCell.wall[NORTH] = !isClear(NORTH);
		currCell.wall[SOUTH] = !isClear(SOUTH);
		currCell.wall[EAST] = !isClear(EAST);
		currCell.wall[WEST] = !isClear(WEST);
		if(x!=0) cell[x-1][y].wall[EAST] = currCell.wall[WEST];
		if(x!=13) cell[x+1][y].wall[WEST] = currCell.wall[EAST];
		if(y!=0) cell[x][y-1].wall[SOUTH] = currCell.wall[NORTH];
		if(y!=13) cell[x][y+1].wall[NORTH] = currCell.wall[SOUTH];
	}
	
	public Cell getCell(int direction)
	{
		switch (direction)
		{
			case NORTH : return cell[x][y-1];
			case SOUTH : return cell[x][y+1];
			case EAST : return cell[x+1][y];
			case WEST : return cell[x-1][y];
		
		}
		return null;
	}
	
	public void rightFirst()
	{
		if(!returning||(returning&&!visitedCells.isEmpty()))
		{
			currCell = cell[x][y];
			currCell.x=x;
			currCell.y=y;
			currCell.visited=true;
			
			if (returning) visitedCells.pop();
			else visitedCells.push(currCell);
			
			if ((x==6&&y==6)||(x==6&&y==7||(x==6&&y==7)||(x==7&&y==6))) 
			{
				finished = true;
				return;
			}
			wallCheck();
			
			if (!currCell.wall[right()]&&(!returning&&!getCell(right()).visited||returning)/*&&getCell(right()).distance!=0*/) 
				{
					heading = right();
					if (!getCell(heading).visited&&returning) returning=false;
				}
			else if (!currCell.wall[heading]&&(!returning&&!getCell(heading).visited||returning)/*&&getCell(heading).distance!=0*/) 
			{
				if (!getCell(heading).visited&&returning) returning=false;
			}
			else if (!currCell.wall[left()]&&(!returning&&!getCell(left()).visited||returning)/*&&getCell(left()).distance!=0*/) 
			{
				heading = left();
				if (!getCell(heading).visited&&returning) returning=false;
			}
			else 
			{
				heading = back();
				returning = true;
			}
		}
		else finished = true;
	}
	
	public void leftFirst()
	{
		currCell = cell[x][y];
		currCell.x=x;
		currCell.y=y;
		currCell.visited=true;
		
		if (returning) visitedCells.pop();
		else visitedCells.push(currCell);
		
		if ((x==6&&y==6)||(x==6&&y==7||(x==6&&y==7)||(x==7&&y==6))) 
		{
			finished = true;
			return;
		}
		wallCheck();
		
		if (!currCell.wall[left()]&&(!returning&&!getCell(left()).visited||returning)) 
			{
				heading = left();
				if (!getCell(heading).visited&&returning) returning=false;
			}
		else if (!currCell.wall[heading]&&(!returning&&!getCell(heading).visited||returning)) 
		{
			if (!getCell(heading).visited&&returning) returning=false;
		}
		else if (!currCell.wall[right()]&&(!returning&&!getCell(right()).visited||returning)) 
		{
			heading = right();
			if (!getCell(heading).visited&&returning) returning=false;
		}
		else 
		{
			heading = back();
			returning = true;
		}
	}
	
	public void printStack()
	{
		Screen.show("returning", ""+returning);
		Screen.show("Heading", heading);
		Screen.show("","**************************");
		for (Cell c : visitedCells)
		{
			Screen.show("", c.x+","+c.y);
		}
		Screen.show("","**************************");
	}
	
	public void test()
	{
		while (!finished)
		{
			rightFirst();
			printStack();
			move();
			//printMaze();
			delay(500);
		}
	}
	
	public void explore()
	{
		while (!finished)
		{
			rightFirst();
			move();
			printMaze();
			delay(500);
		}
		printStack();
	}
	
	public void draw()
	{
		super.draw();
		//g.setColor(Color.RED);
		//if (finished) for(Cell c : visitedCells) circle(c.x*35+17,c.y*35+20,5,true);
		g.setColor(Color.BLACK);
	}
	
	public void move()
	{
		cell[x][y].visited=true;
		super.move();
	}
	
	public void printMaze()
	{
		for(int i=0; i<14; i++)System.out.print("+---");
		System.out.println("+");
		for (int y=0; y<14; y++) 
		{
			for (int x=0; x<14; x++)cell[x][y].printCell();
			System.out.println("|");
			for (int x=0; x<14; x++)cell[x][y].printBottom();
			System.out.println("+");
		}
	}
	
	public static void main(String[] args) 
	{
		MemoryMaze tc;
		GridForm f = new GridForm("Test",10,10,45,80,20,20);
		f.add(tc = new MemoryMaze(),1,1,500/f.colWidth+1,500/f.rowHeight+1);
		tc.loadImage();
		tc.setupCoordinateSystem(0, 500, 0, 500);
		tc.draw();
		tc.repaint();
		tc.explore();
	}
}
