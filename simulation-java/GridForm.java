import java.awt.*;

import javax.swing.*;

import java.awt.event.*;

public class GridForm extends Form implements ComponentListener 
{
	static Toolkit defaultToolkit = Toolkit.getDefaultToolkit();
	static Dimension display = defaultToolkit.getScreenSize();
	
	public int colWidth, rowHeight, cols, rows;
	public int componentCount = 0;
	
	public int maxComponents=200;
	public Component[] components = new Component[maxComponents];
	public Dimension[] componentDimensions = new Dimension[maxComponents];
	public Dimension[] componentPosition = new Dimension[maxComponents];
	
	public GridForm(String title, int x, int y, int width, int height, int cols, int rows)
	{
		super(title, x*display.width/100, y*display.height/100, width*display.width/100, height*display.height/100);
		colWidth = container.getWidth()/cols;
		rowHeight = container.getHeight()/rows;
		this.cols = cols;
		this.rows = rows;
		this.addComponentListener(this);
	}
	
	public void add(Component component,int x, int y, int width, int height)
	{
		super.add(component, x*colWidth, y*rowHeight, width*colWidth, height*rowHeight);
		addComponent(component,x,y,width,height);
	}
	
	public int getCol(Component component)
	{
		return component.getX()/colWidth;
	}
	
	public int getRow(Component component)
	{
		return component.getY()/rowHeight;
	}
	
	public JLabel addLabel(String caption, int col, int row, int width, int height)
	{
		JLabel lbl = new JLabel(caption);
		add(lbl, col, row, width, height);
		addComponent(lbl,col,row,width,height);
		return lbl;
	}
	
	public JTextField addTextField(int alignment, int col, int row,int width, int height)
	{
		JTextField txt = new JTextField();
		addComponent(txt,col,row,width,height);
		txt.setHorizontalAlignment(alignment);
		add(txt, col, row, width, height);
		return txt;
	}
	
	public JTextField addTextField(int col, int row,int width, int height)
	{
		return addTextField(JTextField.LEFT,col,row,width,height);
	}
	
	public JButton addButton(String text, int col, int row, int width, int height)
	{
		JButton btn = new JButton();
		addComponent(btn,col,row,width,height);
		btn.setText(text);
		add(btn, col, row, width, height);
		return btn;
	}
	
	private void addComponent(Component component,int x, int y, int width, int height)
	{
		components[componentCount]=component;
		componentDimensions[componentCount] = new Dimension(width,height);
		componentPosition[componentCount] = new Dimension(x,y);
		componentCount++;
	}
	
	public void componentResized(ComponentEvent e) 
	{
		colWidth = container.getWidth()/cols;
		rowHeight = container.getHeight()/rows;
		for(int a=0; a<componentCount; a++)
		{
			Component component = components[a];
			int x = componentPosition[a].width*colWidth;
			int y = componentPosition[a].height*rowHeight;
			int width = componentDimensions[a].width*colWidth;
			int height = componentDimensions[a].height*rowHeight;
			component.setBounds(x, y, width, height);
		}
	}
	
	public void componentHidden(ComponentEvent arg0) {}
	public void componentMoved(ComponentEvent arg0) {}
	public void componentShown(ComponentEvent arg0) {}

	public static void main(String[] args)
	{
		new GridForm("test", 10, 10, 50, 50, 5, 5);
	}
	
}
