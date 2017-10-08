import java.net.*;
import java.io.*;
public class BlueThread extends Thread{
	BlueServer srv;
	Socket sock;
	DataInputStream din;
	DataOutputStream dout;
	String ipAddr;
	public BlueThread(BlueServer se, Socket sc){
		try{
			srv=se;
			sock = sc;
			din = new DataInputStream(sock.getInputStream());
			dout = new DataOutputStream(sock.getOutputStream());
			ipAddr = sock.getInetAddress().toString();
			System.out.println(ipAddr +"Connect");
			sendMsg(ipAddr + "Connect");
		}
	catch (Exception e)
	 {
		System.out.println(e.toString() +"111");
	 }
	}
	public void sendMsg(String msg){
		try{
			dout.writeUTF(msg);
		}
		catch (Exception e)
		{
			System.out.println(e.toString()+"222");
		}
	}
	public void run(){
		String str;
		try{
			while (true){
				str = din.readUTF();
				srv.sendAll(str);
			}
		}
		catch (Exception e){
			System.out.println(e.toString()+"333");
		}
	}
}

