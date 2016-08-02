#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#include <DHT.h>

//DHT constants
#define DHTTYPE DHT11 // DHT11 or DHT22
#define DHTPIN 0

//WiFi network constants
const char* ssid = "<network name>";
const char* password = "<network password>";

//Objects and variables
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE,0); //Last parameter is ignored
Adafruit_BMP280 bmp; //I2C
float h, t, t1, p;

// ThingSpeak constants
byte postServer[]  = {184,106,153,149}; //ThingSpeak server IP
String writeAPIKey = "<write key>"; //Your channel's write key
const int updateInterval = 20*1000; //Interval in milliseconds


//POSTS data to Thingspeak
void POST(String data){
  if (client.connect(postServer, 80)){ //Connect to the ThingSpeak server
    //print a HTTP POST header to the EthernetClient
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");
    client.print(data);
    
    //client.flush();
    Serial.println("Data posted");
  }
}

//Reads DHT sensor data
boolean readDHT(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return false;
  }else{
    Serial.print("t:");
    Serial.println(t);
    Serial.print("h:");
    Serial.println(h);
    Serial.println("dht read");
    return true;
  }
}

//Reads BMP sensor data
boolean readBMP(){
  t1 = bmp.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(t1);
  Serial.println(" *C");

  p = bmp.readPressure();
  Serial.print("Pressure = ");
  Serial.print(p/100);
  Serial.println(" hPa");
  
  Serial.println("bmp read");
  return true;
}

//Prepares sensor data to be sent to Thingspeak and waits for the reply
void post2cloud(){
  String postString = "field1="+String(t);
  postString += "&field2="+String(t1);
  postString += "&field3="+String(h);
  postString += "&field4="+String(p);
  POST(postString);

  //Wait for server response
  while(!client.available()){}
  //Print the server response to serial
  while(client.available()){
    char c = client.read();
    Serial.print(c);
  }
  
  //Stop the client to prepare for the next connection
  client.stop();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Serial ready.");
  dht.begin();
  readDHT();

  if (!bmp.begin(0x76)) { //0x76
    Serial.println("Could not find a valid BMP280 sensor, check wiring or address!");
    while(1);
  }
  delay(50);
  readBMP();

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() { 
  //Read DHT
  readDHT();
  
  //Read BMP
  readBMP();

  //POST Thingspeak
  post2cloud();
  
  Serial.println("-----------------------\n\n");
  
  //Wait some time before repeating the process
  delay(updateInterval);
}
