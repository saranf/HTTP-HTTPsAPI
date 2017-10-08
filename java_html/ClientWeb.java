import java.awt.*;
import javax.swing.*;
import java.io.*;
import java.util.*;
import java.applet.*;
import java.awt.event.*;
import java.net.*;

public class ClientWeb extends JApplet implements Runnable {
	DataInputStream din;
	DataOutputStream dout;

	Thread t;
	Socket sock;
	String str;
	Vector v;
	JButton btn, btn1, btn2;

	public void init() {
		this.setLayout(null);
		btn = new JButton();
		btn1 = new JButton("LED ON");
		btn2 = new JButton("LED OFF");
		btn.setBounds(0,0,500,300);
		btn1.setBounds(0,300,250,100);
		btn2.setBounds(250,300,250,100);
		this.add(btn);
		this.add(btn1);
		this.add(btn2);
		setSize(500,420);
		setVisible(true);
		str = "";
		btn1.addActionListener(new Btn1Handler());
		btn2.addActionListener(new Btn2Handler());
		t = new Thread(this);
		t.start();
	}

	public void run() {
		try {
			sock = new Socket("192.168.1.3", 7777);
			din = new DataInputStream(sock.getInputStream());
			dout = new DataOutputStream(sock.getOutputStream());
			while(true) {
				str = din.readUTF();
				btn.setLabel(str);
			}
		} catch(Exception e) {
			System.out.println(e.toString() + "11");
		}
	}

	public void sendMsg(String msg) {
		try {
			dout.writeUTF(msg);
		} catch ( Exception e) {
			System.out.println(e.toString() + "12");
		}
	}

	class Btn1Handler implements ActionListener {
		public void actionPerformed(ActionEvent e)
		{
			sendMsg("1");
		}
	}

	class Btn2Handler implements ActionListener {
		public void actionPerformed(ActionEvent e)
		{
			sendMsg("0");
		}
	}
}
