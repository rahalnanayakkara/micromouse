import java.util.Stack;

public class FloodfillMaze extends MemoryMaze 
{
	Stack<Cell> flood = new Stack<Cell>();
	Cell check;
	int minNeighbour = Integer.MAX_VALUE, minValue = Integer.MAX_VALUE;
	int currValue;
	
	public FloodfillMaze()
	{
		super();
		for (int i=0; i<=6; i++) for(int j=0; j<=6; j++)
		{
			cell[i][j].distance = 12-i-j;
			cell[i+7][j].distance = 6-j+i;
			cell[i][j+7].distance = 6+j-i;
			cell[i+7][j+7].distance = j+i;
		}
	}
	
	public void lowestFirst()
	{
		minValue = Integer.MAX_VALUE;
		currCell = cell[x][y];
		
		//if(visitedCells.contains(currCell)) visitedCells.pop();
		//else visitedCells.push(currCell);
		
		if ((x==6&&y==6)||(x==6&&y==7||(x==6&&y==7)||(x==7&&y==6))) 
		{
			finished = true;
			return;
		}
		
		wallCheck();
		floodFill();
		currValue = currCell.distance;
		
		//for(Cell c : visitedCells)
		//	if(c.distance<minValue) minValue = c.distance;
		
		cellConforms(currCell);
		
		
		if(!currCell.wall[heading]&&getCell(heading).distance==minNeighbour&&minNeighbour<=minValue);
		else if(!currCell.wall[right()]&&getCell(right()).distance==minNeighbour&&minNeighbour<=minValue) heading = right();
		else if(!currCell.wall[left()]&&getCell(left()).distance==minNeighbour&&minNeighbour<=minValue) heading = left();
		else heading = back();
	}

	public void lowestFirst2()
	{
		minValue = Integer.MAX_VALUE;
		currCell = cell[x][y];
		
		//if(visitedCells.contains(currCell)) visitedCells.pop();
		//else visitedCells.push(currCell);
		
		if (x==13&&y==0) 
		{
			finished = true;
			heading = back();
			return;
		}
		
		wallCheck();
		floodFill();
		currValue = currCell.distance;
		
		//for(Cell c : visitedCells)
		//	if(c.distance<minValue) minValue = c.distance;
		
		cellConforms(currCell);
		
		
		if(!currCell.wall[heading]&&getCell(heading).distance==minNeighbour&&minNeighbour<=minValue);
		else if(!currCell.wall[right()]&&getCell(right()).distance==minNeighbour&&minNeighbour<=minValue) heading = right();
		else if(!currCell.wall[left()]&&getCell(left()).distance==minNeighbour&&minNeighbour<=minValue) heading = left();
		else heading = back();
		return;
	}
	
	public void floodFill()
	{
		int n=0;
		flood.push(currCell);

		//Comment THIS
		if (currCell.wall[NORTH]&&y!=0&&getCell(NORTH).distance!=0) flood.push(getCell(NORTH));
		if (currCell.wall[SOUTH]&&y!=13&&getCell(SOUTH).distance!=0) flood.push(getCell(SOUTH));
		if (currCell.wall[EAST]&&x!=13&&getCell(EAST).distance!=0) flood.push(getCell(EAST));
		if (currCell.wall[WEST]&&x!=0&&getCell(WEST).distance!=0) flood.push(getCell(WEST));
		
		while (!flood.isEmpty())
		{
			n++;
			check = flood.pop();
			int x = check.x;
			int y = check.y;
			if(!cellConforms(check))
			{
				if (check.distance!=0) check.distance=(check.distance<98)?minNeighbour+1:99;
				if (!check.wall[NORTH]) flood.push(cell[x][y-1]);
				if (!check.wall[SOUTH]) flood.push(cell[x][y+1]);
				if (!check.wall[EAST]) flood.push(cell[x+1][y]);
				if (!check.wall[WEST]) flood.push(cell[x-1][y]);
			}
			if (n>200) break;
		}
	}
	
	public void virtualWalls()
	{
		for(int x=1; x<13; x++) for(int y=1; y<13; y++)
		{
			if (!cell[x+1][y].visited&&cell[x+1][y].distance!=0) 
			{
				cell[x][y].wall[EAST] = true;
				cell[x+1][y].wall[WEST] = true;
			}
			if (!cell[x-1][y].visited&&cell[x-1][y].distance!=0) 
			{
				cell[x][y].wall[WEST] = true;
				cell[x-1][y].wall[EAST] = true;
			}
			if (!cell[x][y-1].visited&&cell[x][y-1].distance!=0) 
			{
				cell[x][y].wall[NORTH] = true;
				cell[x][y-1].wall[SOUTH] = true;
			}
			if (!cell[x][y+1].visited&&cell[x][y+1].distance!=0) 
			{
				cell[x][y].wall[SOUTH] = true;
				cell[x][y+1].wall[NORTH] = true;
			}
		}
	}
	
	//returns true if cell value is one more than minimum adjacent neighbour
	public boolean cellConforms(Cell thisCell)
	{
		minNeighbour=Integer.MAX_VALUE;
		int x = thisCell.x;
		int y = thisCell.y;
		
		if(!thisCell.wall[NORTH]&&cell[x][y-1].distance<minNeighbour)
			minNeighbour=cell[x][y-1].distance;
		if(!thisCell.wall[SOUTH]&&cell[x][y+1].distance<minNeighbour)
			minNeighbour=cell[x][y+1].distance;
		if(!thisCell.wall[EAST]&&cell[x+1][y].distance<minNeighbour)
			minNeighbour=cell[x+1][y].distance;
		if(!thisCell.wall[WEST]&&cell[x-1][y].distance<minNeighbour)
			minNeighbour=cell[x-1][y].distance;
		if(thisCell.wall[NORTH]&&thisCell.wall[SOUTH]&&thisCell.wall[WEST]&&thisCell.wall[EAST])
			minNeighbour=98;
		if (thisCell.distance==minNeighbour+1)
			return true;
		else return false;
	}
	
	public void draw()
	{
		super.draw();
		for(int x=0; x<14; x++) for(int y=0; y<14; y++)
			text(cell[x][y].distance+"",x*35.1+16.5,y*36+20);
		
	}
	
	public void explore()
	{
		
		while(!finished)
		{
			lowestFirst();
			floodFill();
			printMaze();
			
			//Keyboard.readString("");
			
			move();
			
			delay(500);
			//Keyboard.readString("");
		}
		
		//Screen.show("", "DONE");
		heading = back();
		wallCheck();
		move();
		wallCheck();
		move();
		printMaze();
		//printStack();
		//super.explore();
	}
	
	public void lowestFirstImproved()
	{
		if(!returning||(returning&&!visitedCells.isEmpty()))
		{
			minValue = Integer.MAX_VALUE;
			
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
			floodFill();
			
			currValue = currCell.distance;
			
			cellConforms(currCell);
			
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
	
	public void test()
	{
		/*
		for(int x=0; x<14; x++) for(int y=0; y<14; y++)
		{
			setX(x);
			setY(y);
			currCell = cell[x][y];
			wallCheck();
		}
		setX(13);
		setY(0);
		printMaze();
		Keyboard.readString("");
		for(int x=0; x<14; x++) for(int y=0; y<14; y++)
		{
			setX(x);
			setY(y);
			currCell = cell[x][y];
			floodFill();
		}
		printMaze();
		*/
		
		while(!finished)
		{
			lowestFirst();
			// lowestFirstImproved();
			//printMaze();
			
			floodFill();

			move();
			
			delay(500);
		}
		
		for(int x=0; x<14; x++) for(int y=0; y<14; y++)
			cell[x][y].distance = 13-x+y;

		int tempX=x,tempY=y;
		for(int x=0; x<14; x++) for(int y=0; y<14; y++)
		{
			setX(x);
			setY(y);
			currCell = cell[x][y];
			floodFill();
		}
		setX(tempX);
		setY(tempY);

		finished = false;
		heading = back();

		while(!finished)
		{
			lowestFirst2();
			// lowestFirstImproved();
			//printMaze();
			floodFill();
			if (finished) break;
			move();
			
			delay(500);
		}

		for (int i=0; i<=6; i++) for(int j=0; j<=6; j++)
		{
			cell[i][j].distance = 12-i-j;
			cell[i+7][j].distance = 6-j+i;
			cell[i][j+7].distance = 6+j-i;
			cell[i+7][j+7].distance = j+i;
		}

		virtualWalls();
		//printMaze();
		tempX=x;
		tempY=y;
		for(int x=0; x<14; x++) for(int y=0; y<14; y++)
		{
			setX(x);
			setY(y);
			currCell = cell[x][y];
			floodFill();
		}
		setX(tempX);
		setY(tempY);
		//printMaze();
		repaint();
	}
	
	public static void main(String[] args) 
	{
		FloodfillMaze tc;
		GridForm f = new GridForm("Test",10,10,45,80,20,20);
		f.add(tc = new FloodfillMaze(),1,1,500/f.colWidth+1,500/f.rowHeight+1);
		tc.loadImage();
		tc.setupCoordinateSystem(0, 500, 0, 500);
		tc.draw();
		tc.repaint();
		//tc.explore();
		tc.test();
	}
}
