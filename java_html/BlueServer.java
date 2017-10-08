import java.io.IOException;
import java.net.*;
import java.util.*;
import com.pi4j.io.gpio.*;
import com.pi4j.io.gpio.event.*;
import com.pi4j.io.serial.*;

public class BlueServer{
	ServerSocket serv;
	Socket sock;
	Vector v;
	final GpioController gpio;
	final GpioPinDigitalOutput pin;
        final GpioPinDigitalInput sw;
        final Serial serial;
        boolean flag = false;
	public BlueServer(){
			gpio = GpioFactory.getInstance();
			serial = SerialFactory.createInstance();
			pin =  gpio.provisionDigitalOutputPin(RaspiPin.GPIO_08,"MyLED",PinState.HIGH);
			v = new Vector();
			sw = gpio.provisionDigitalInputPin(RaspiPin.GPIO_07,PinPullResistance.PULL_DOWN);
			sw.addListener(new GpioPinListenerDigital(){
			@Override
			public void handleGpioPinDigitalStateChangeEvent(GpioPinDigitalStateChangeEvent e) {
				// TODO Auto-generated method stub
				if(e.getState()== PinState.HIGH){
					flag = !flag;
					if(flag) {
						sendAll("1");
						} else {
						sendAll("0");
						}
					}
				
					}
			});
			serial.addListener(new SerialDataEventListener() {
			 public void dataReceived(SerialDataEvent event){
			  String str;
				try {
					// 
					str = event.getAsciiString();
					 if (str.equals("1")) {
						    sendAll("1");
					 } else {
						    sendAll("0");
					 }
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			 
			      }
			});
 
			try{
			   serv= new ServerSocket(7777);
			serial.open(Serial.DEFAULT_COM_PORT,115200);

			   while(true){
					sock = serv.accept();
					BlueThread client = new BlueThread(this, sock);
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
			try {
				serial.write("f");
			} catch (IllegalStateException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		else
			try {
				serial.write("s");
			} catch (IllegalStateException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		for(int i=0;i<v.size();i++){
					BlueThread client= (BlueThread)v.get(i);
					client.sendMsg(msg);
		}
	}
	public static void main (String arges[]){

		BlueServer sm= new BlueServer();
	}
}
