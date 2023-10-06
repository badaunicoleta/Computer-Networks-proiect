
#include <Servo.h>;
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <HttpClient.h>

const char* ssid     = "Elena's iPhone";
const char* password = "heythere";
const char* host = "172.20.10.2"; // IPv4 adress
const int   port = 8080;     //port Domoticz
const int   watchdog = 6000; // Frequency of sending data to Domoticz
unsigned long previousMillis = millis();  

HTTPClient http;
WiFiClient wifiClient;
String url;

Servo motorPin;
int sensor_pin_light = A0; // Photoresistor input at Analog PIN A0
int output_value_light = 0;
unsigned long current_ms=0;


void read_sensor()
{

  Serial.println("Reading From the Sensor ...");
  output_value_light= analogRead(sensor_pin_light);
 //output_value_light = (output_value_light/1024); 
 Serial.println("Light : ");
 Serial.println(output_value_light);
 delay(3000);
}

void roof()
{
  if (output_value_light<20)
  {  
  motorPin.write(90);  
  delay(1000); 
  motorPin.write(0);
  delay(1000); 
  }
}


void setup()         // put your setup code here, to run once:
{ 
  
  Serial.begin(9600);
  Serial.println("The board has started");                 
  motorPin.attach(D7);
  delay(2000);
  current_ms=millis();

  //Connecting to Wifi

   Serial.println("Connecting Wifi...");
 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)   //connecting to wifi
      {
        delay(500);
        Serial.print(".");                                     
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.print(WiFi.localIP()); 
      }

      delay(10);
}


void loop()
{
  read_sensor();
  roof();
  unsigned long currentMillis = millis();
  if ( currentMillis - previousMillis > watchdog ) 
  {
    previousMillis = currentMillis;
    if(WiFi.status() != WL_CONNECTED)
      { 
        Serial.println("WiFi not connected !");
      } 
    else 
      { Serial.println("Send data to Domoticz");
        //int sensorValue = analogRead(A0);  
        // read the input on analog pin 0
        //int output = sensorValue * (100 / 1023.0);  
        // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 100V)
        //request command
        url = "/json.htm?type=command&param=udevice&idx=1&nvalue=0&svalue=";  
        url += String(output_value_light); url += ";";
        sendDomoticz(url);
       }
    }
}

void sendDomoticz(String url) //updating data
{
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  http.begin(wifiClient,host,port,url);
  int httpCode = http.GET();
    if (httpCode)
    { if(httpCode == 200)
     {
        String payload = http.getString();
        Serial.println("Domoticz response "); 
        Serial.println(payload);
      
    }
    }
  else 
    //Serial.println("Not connected to Domoticz");
    Serial.println (httpCode);
   // Serial.println("Nope, closing connection");
  
  http.end();
}


 
