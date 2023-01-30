public class Screen
{
	public static void display(String datum, int width) 
	{
		System.out.print(datum);
		for(int i=datum.length();i<=width;i++) System.out.print(" ");
	}

	public static void display(long datum, int width) 
	{
		String s=String.valueOf(datum);
		for(int i=s.length();i<=width;i++) System.out.print(" ");
		System.out.print(datum+" ");
	}
	
	public static void display(char c, int width)
	{
		display(""+c,width);
	}
		
	public static void display(float datum, int width)
	{
		String s = String.valueOf(datum);
		for(int i=s.length();i<=width;i++) System.out.print(" ");
		System.out.print(datum);
	}
		
	public static void show(String message, long datum)
	{
		  System.out.println(message+" "+datum);
	}
	
	public static void show(String nessage, String datum)
	{
		System.out.println(nessage+" "+datum);
	}
	
	public static void show(String nessage, float datum)
	{
		System.out.println(nessage+" "+datum);
	}
		
	public static void show(String nessage, double datum)
	{
		System.out.println(nessage+" "+datum);
	}

	public static void nextLine() 
	{
		show("","");
	}
	
	public static void display(int...a)
	{
		for(int z: a) Screen.display(z+"\t\t\t", 0);
		nextLine();
	}
	
	public static void display(String...s)
	{
		for(String string : s) Screen.display(string+"\t\t\t", 0);
		nextLine();
	}
	
	public static void display(char...c)
	{
		for(int x=0; x<c.length; x++) Screen.display(c[x]+" ", 0);
		nextLine();
	}
}