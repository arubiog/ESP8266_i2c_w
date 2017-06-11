/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/r0 will set the GPIO2 low,
 *    http://server_ip/r1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */ 
#include <Wire.h> 
#include <ESP8266WiFi.h>

#define i2cT0 0x48
#define i2cT1 0x49
#define i2cT2 0x4A
#define i2cT3 0x4B
#define i2cT4 0x4C
#define i2cT5 0x4D

const char* ssid = "horizonte";
const char* password = "Z0002CF9C470E";
const char* ssid1 = "vodafoneC350";
const char* password1 = "numancia73";
const char* ssid2 = "albaguest";
const char* password2 = "guest2010";

int buzzpin=2;
int ledpin=16;

int in1pin=0;
int Rele1pin=15;

int TPin = 4;    // select the input pin for the Temerature
int ClockPin = 5; // select the input pin for the Temerature

int adcpin=A0;

bool live=0;
int Temp0=-2;
int Temp1=-2;
int Temp2=-2;
int Temp3=-2;
int Temp4=-2;
int Temp5=-2;
int Adc=-1;
bool Rele1_value=0;
bool buzz_value=1;
bool led_value=0;
bool live_value=0;

IPAddress espIP;

String espMAC;
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

// FUNCTIONS
int I2Ctemp(int ad,int delay1) {   //reemplazar por la direccion en hex o Dec
  int temperature = 0;      
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.beginTransmission(ad);
  //Wire.write(0x00); // 
  //Wire.write(0x20); // CTRL_REG1 (20h)
  Wire.requestFrom(ad, 2);
  temperature = Wire.read();
  Wire.endTransmission();
  delay(delay1);
  return(temperature);  
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  // prepare GPIO
  pinMode(Rele1pin, OUTPUT);
  digitalWrite(Rele1pin, LOW);  
  pinMode(buzzpin, OUTPUT);
  digitalWrite(buzzpin, LOW);  
  pinMode(ledpin, OUTPUT);
  digitalWrite(ledpin, LOW);  
  
  //i2c
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.beginTransmission(i2cT0);
  Wire.beginTransmission(i2cT1);
  Wire.beginTransmission(i2cT2);
  Wire.beginTransmission(i2cT3);
  Wire.beginTransmission(i2cT4);
  Wire.beginTransmission(i2cT5);
  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(ledpin, false);
    delay(500);
    digitalWrite(ledpin, true);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(buzzpin, 1);
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  espIP=WiFi.localIP();
  espMAC=WiFi.softAPmacAddress();
  Serial.println(espIP);
  Serial.println(WiFi.BSSIDstr());
  Serial.println(espMAC); 
}


void loop() {
  Temp0 = I2Ctemp(i2cT0, 100);   // read the value from the sensor:
  Temp1 = I2Ctemp(i2cT1, 100);   // read the value from the sensor:
  Temp2 = I2Ctemp(i2cT2, 100);   // read the value from the sensor:
  Temp3 = I2Ctemp(i2cT3, 100);   // read the value from the sensor:
  Temp4 = I2Ctemp(i2cT4, 100);   // read the value from the sensor:
  Temp5 = I2Ctemp(i2cT5, 100);   // read the value from the sensor:
  
  delay(1000);
  Adc = analogRead(in1pin);
  delay(20);   
 // Set GPIO2 according to the request
  digitalWrite(Rele1pin, Rele1_value);
  digitalWrite(buzzpin, buzz_value);
  if (live_value){
    if (live) {
      digitalWrite(ledpin, true);
      live=0;
    }else{
      digitalWrite(ledpin, false);
      live=1;
    }
  }else{
      digitalWrite(ledpin, true);
  }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {

    return;
  }
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()) Serial.println("wait");
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  // Match the request
  int rele1pin=13;


  if (req.indexOf("/rl10") != -1) {Rele1_value=0;} //led rele
  else if (req.indexOf("/rl11") != -1) {Rele1_value=1;}
  else if (req.indexOf("/buzz0") != -1) {buzz_value=1;}
  else if (req.indexOf("/buzz1") != -1) {buzz_value=0;}
  else if (req.indexOf("/live0") != -1) {live_value=0;}
  else if (req.indexOf("/live1") != -1) {live_value=1;}  
  else if (req.indexOf("/") != -1) {delay(10);} //led rele
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  client.flush();
  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
  s += (espIP);
  s += "<br/>";
  s += (Temp0);
  s += "<br/>";
  s += (Temp1);
  s += "<br/>";
  s += (Temp2);
  s += "<br/>";
  s += (Temp3);
  s += "<br/>";
  s += (Temp4);
  s += "<br/>";
  s += (Temp5);
  s += "<br/>";
  s += (Adc);
  s += "<br/>";
  s += (Rele1_value);
  s += "<br/>";
    s += (buzz_value);
  s += "<br/>";
  s += "</html>\n";
  // Send the response to the client
  client.print(s);
}
