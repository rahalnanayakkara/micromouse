import java.awt.*;
import javax.swing.*;

public class Form extends JFrame 
{
	public Container container;
	
	public Form(String title, int x, int y, int width, int height)
	{
		super(title);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(x,y,width,height);
		container = getContentPane();
		container.setLayout(null);
		setVisible(true);
	}
	
	public void add(Component component, int x, int y, int width, int height)
	{
		container.add(component);
		component.setBounds(x, y, width, height);
	}

	public static void main(String[] args)
	{
		new Form("as",10,10,10,10);
	}
}
