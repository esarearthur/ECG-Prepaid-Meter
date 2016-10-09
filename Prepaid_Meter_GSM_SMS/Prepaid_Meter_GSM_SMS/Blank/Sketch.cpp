#include <Arduino.h>
#include <GSM.h>

#define relayPin 12

GSM gsmAccess;
GSMClient client;
GPRS gprs;
GSM_SMS sms;

char senderNumber[20];
char *PINNUMBER = "";
char *GPRS_APN = "internet";
char *GPRS_LOGIN = "";
char *GPRS_PASSWORD = "";
char *apiKey = "VKKVT14I2U573BPZ";
char server[] = "api.thingspeak.com";
char path[] = "/";
int port = 80;

void SendDataToServer(){
	bool resp;
	
	String getStr = "GET /update?api_key=";
	getStr += apiKey;
	getStr +="&field1=";
	getStr += 100;
	getStr += "\r\n\r\n";
	
	if(client.connect(server,port))
	{
		client.print("AT+CIPSEND=");
		client.print(getStr.length());
		client.print("\r");
		
		if(client.find(">"))
		{
			client.print(getStr);
			client.write(26);
			client.print("\r");
		}
	}
	client.flush();
}

void SendSMS(char remoteNum[], char txtMsg[]){
	sms.beginSMS(remoteNum);
	sms.print(txtMsg);
	sms.endSMS();
}

String ReadSMS(){
	char c;
	String out;
	if (sms.available()) {

		// Get remote number
		sms.remoteNumber(senderNumber, 20);
		Serial.println(senderNumber);

		// An example of message disposal
		// Any messages starting with # should be discarded
		if (sms.peek() == '#') {
				sms.flush();
		}
		
		// Read message bytes and print them
		while (c = sms.read()) {
			Serial.write(c);
			out += c;
		}
		Serial.println("\r");
		
		// Delete message from modem memory
		sms.flush();
		
		return out;
	}
}

void setup() {
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);	
	
	Serial.begin(9600);
	boolean notConnected = true;
	
	pinMode(relayPin, OUTPUT);
	digitalWrite(relayPin, LOW);
	
	while(notConnected){
		if(gsmAccess.begin(NULL,false,true) == GSM_READY 
			&& gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)
			notConnected = false;
		else{
			delay(1000);
		}
	}
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);	
}

char remoteNumber[20];
void loop() {
	SendDataToServer();
	
	String readString = ReadSMS();
	
	if (readString.length() > 0 && String(senderNumber).indexOf("267766253") >= 0)
	{
		if(readString.indexOf("on") >= 0)
		{
			digitalWrite(relayPin, LOW);
			Serial.println("RELAY ON");
		}
		
		if(readString.indexOf("off") >= 0)
		{
			digitalWrite(relayPin, HIGH);
			Serial.println("RELAY OFF");
		}
	}
	delay(2000);
}
