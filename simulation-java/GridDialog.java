import java.awt.*;

import javax.swing.*;

public class GridDialog extends JDialog 
{
	static Font defaultFont = new Font("Sans serif",Font.BOLD,14);
	static Dimension display = Toolkit.getDefaultToolkit().getScreenSize();
	Font font = defaultFont;
	Container panel;
	int colWidth, rowHeight;
	
	public GridDialog(JFrame owner, String title, int x, int y, int width, int height, int cols, int rows)
	{
		super(owner, title, true);
		setBounds( x*display.width/100, y*display.height/100, width*display.width/100, height*display.height/100);
		panel = getContentPane();
		panel.setLayout(null);
		colWidth = (getWidth()-(owner.getWidth()-owner.getContentPane().getWidth()))/cols;
		rowHeight = (getHeight()-(owner.getHeight()-owner.getContentPane().getHeight()))/rows;
		setDefaultCloseOperation(HIDE_ON_CLOSE);
	}
	
	public void add (Component c, int col, int row, int width, int height)
	{
		panel.add(c);
		c.setFont(font);
		c.setBounds(col*colWidth, row*rowHeight, width*colWidth, height*rowHeight);
	}
	public JLabel addLabel(String caption, int col, int row, int width, int height)
	{
		JLabel lbl = new JLabel(caption);
		add(lbl, col, row, width, height);
		return lbl;
	}
}
