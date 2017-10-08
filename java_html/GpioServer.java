import java.net.*;
import java.util.*;
import com.pi4j.io.gpio.*;
import com.pi4j.io.gpio.event.*;
public class GpioServer{
	ServerSocket serv;
	Socket sock;
	Vector<GpioThread> v;
	final GpioController gpio;
	final GpioPinDigitalOutput pin;
	public GpioServer(){
			gpio = GpioFactory.getInstance();
			pin =  gpio.provisionDigitalOutputPin(RaspiPin.GPIO_08,"MyLED",PinState.HIGH);
			v = new Vector<GpioThread>();
			try{
			   serv= new ServerSocket(7777);
			   while(true){
					sock = serv.accept();
					GpioThread client = new GpioThread(this, sock);
					v.addElement(client);
					client.start();
					}
			}
			catch (Exception e){
					System.out.println(e.toString());
			}
	}
	void sendAll(String msg)
	{
		if(msg.equals("1"))
		pin.high();
		else
		   pin.low();
		for(int i=0;i<v.size();i++){
					GpioThread client= v.get(i);
					client.sendMsg(msg);
		}
	}
	public static void main (String arges[]){
		GpioServer sm= new GpioServer();
	}
}
