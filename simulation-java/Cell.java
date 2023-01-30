public class Cell 
{
	public boolean visited;
	public boolean[] wall = new boolean[5];
	public int distance, x, y;
	
	public void printCell()
	{
		if (wall[MazeCanvas.WEST])
			System.out.print("|");
		else
			System.out.print(" ");
		if (visited)
			System.out.print("*");
		else
			System.out.print(" ");
		if (distance>9)
			System.out.print(distance);
		else
			System.out.print(distance+" ");
	}
	
	public void printBottom()
	{
		System.out.print("+");
		if (wall[MazeCanvas.SOUTH])
			System.out.print("---");
		else
			System.out.print("   ");
	}
}
