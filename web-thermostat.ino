//Jesus that's a lot of libraries
#include <TimeLib.h> 
#include <ESP8266WiFi.h>  
#include <WiFiUdp.h>          
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <FS.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <SPI.h>

//Variables for delay function
unsigned long previousMillis = 0;
const long interval = 2000; 


//Set up sensors
#define DHTTYPE   DHT22       // DHT type (DHT11, DHT22)
#define DHTPIN 2         // Broche du DHT / DHT Pin
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;

//Set up variables for the timer
int startHour = 16;
int startMin = 55;
int endHour = 16;
int endMin = 56;
int startInt = 0;
int endInt = 120;
bool turnedOn = true;
bool heatOn = false;
bool timerEnable = true;


//Start Server
ESP8266WebServer server(80); 

//Start WebSocket Server
WebSocketsServer webSocket = WebSocketsServer(81);

//Wifi Details
const char* ssid     = "VM555949-2G";
const char* password = "mtfjpzwp";

//set boiler pin and on/off flag
int boiler = 14;
bool boilerOn = "False";


//Variables for sensor values
int tempInt = 35;
float   t = 0 ;
float   h = 0 ;
float   p = 0;
String tempReply="Temp34";
String timeReply="Time0,2";
String startReply="Start900";
String endReply="End1200";
String realTemp = "R24.6";
String humidity = "H48.5";
String pressure = "P1014.3";

//Stupid variables to hold value prefix characters that shouldn't be necessary
String prefixT = "R";
String prefixH = "H";
String prefixP = "P";


// NTP Server:
IPAddress timeServer(91, 206, 8, 36); // 0.at.pool.ntp.org
const int timeZone = 1;     // Central European Time
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets



//handle web socket events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) 
    {
        case WStype_DISCONNECTED:
            
            break;
        case WStype_CONNECTED:
         {
                IPAddress ip = webSocket.remoteIP(num);   
         }
            
        break;
        case WStype_TEXT:
        {
            String text = String((char *) &payload[0]); //recieve text value


            //handle boiler override button messages
            if(text=="Boiler on"){
            heatOn = true;
            Serial.println("Boiler turned ON");
            webSocket.sendTXT(num, "Boiler turned ON", lenght);
            }
            if(text=="Boiler off"){
            heatOn = false;
            Serial.println("Boiler turned OFF");
            webSocket.sendTXT(num, "Boiler turned OFF", lenght);
            }

            //Handle temp setting messages
           if(text.startsWith("SetTemp")){


            // confirm receipt of temp value and store it as an integer
            String tempVal=(text.substring(text.indexOf("SetTemp")+7,text.length())); 
            tempReply="Temp"+ tempVal; 
            Serial.print("Temp set to: ");
            Serial.println(tempVal);
            webSocket.sendTXT(num, reinterpret_cast<const uint8_t*>(&tempReply[0]), 3);
            tempInt = tempVal.toInt();
            }

            if(text.startsWith("SetTime")){
            //Recieve time value and confirm receipt
            String timeVal=(text.substring(text.indexOf("SetTime")+7,text.length())); 
            timeReply="Time"+ timeVal;
            Serial.println(timeReply);
            webSocket.sendTXT(num, reinterpret_cast<const uint8_t*>(&timeReply[0]), timeReply.length());

            //Separate into start and end values
            String startStr=(timeVal.substring(0, timeVal.indexOf(",")));
            String endStr=(timeVal.substring(timeVal.indexOf(",")+1, timeVal.length()));

            // Convert start time into hours and minutes
            if (startStr.indexOf(".")>0){
              String hourStr = (startStr.substring(0, timeVal.indexOf(".")+1));
              String minStr = (startStr.substring(startStr.indexOf(".")+1, startStr.length()));
              startHour = hourStr.toInt();
              
              if (minStr == ("25")){
                 minStr = ("15");
                 startMin = minStr.toInt();
              }
              else if (minStr == ("5")){
                 minStr = ("30");
                 startMin = minStr.toInt();
              }
              else{
                 minStr = ("45");
                 startMin = minStr.toInt();
              }
            }
            else{
              startHour = startStr.toInt();
              startMin = 0; 
            }
              // Convert end time into hours and minutes
              if (endStr.indexOf(".")>0){
              String hourStr = (endStr.substring(0, timeVal.indexOf(".")+1));
              String minStr = (endStr.substring(endStr.indexOf(".")+1, endStr.length()));
              endHour = hourStr.toInt();
              if (minStr == ("25")){
                 minStr = ("15");
                 endMin = minStr.toInt();
              }
              else if (minStr == ("5")){
                 minStr = ("30");
                 endMin = minStr.toInt();
              }
              else{
                 minStr = ("45");
                 endMin = minStr.toInt();
              } 
            }
            else{
              endHour = endStr.toInt();
              endMin = 0; 

            }
              //Debug messages for confirming time setting
              Serial.print("Start time set to: ");
              Serial.print(startHour);
              Serial.print(":");
              Serial.println(startMin);           
              Serial.print("End time set to: ");
              Serial.print(endHour);
              Serial.print(":");
              Serial.println(endMin);
            }
          

           //Check for socket open message
           if(text.startsWith("Socket")){
            String openmsg = text;
            //Serial.println(openmsg);
            Serial.println("Sending Variables");
            
            //send value for temperature control
            webSocket.sendTXT(num, reinterpret_cast<const uint8_t*>(&tempReply[0]), tempReply.length());
            webSocket.sendTXT(num, reinterpret_cast<const uint8_t*>(&timeReply[0]), timeReply.length());
            //send sensor values
            webSocket.sendTXT(num, (reinterpret_cast<const uint8_t*>(&realTemp[0])), 5);
            webSocket.sendTXT(num, reinterpret_cast<const uint8_t*>(&humidity[0]), 5);
            webSocket.sendTXT(num, reinterpret_cast<const uint8_t*>(&pressure[0]), 7);
           }

/*
           if(text.startsWith("z")){
            
            String zVal=(text.substring(text.indexOf("z")+1,text.length())); 
            int zInt = zVal.toInt();
             
            Serial.println(zVal);
            webSocket.sendTXT(num, " changed", lenght);
           }

 */
           if(text=="RESET"){
             Serial.println("reset");
           }
        }
        
            
           
        webSocket.sendTXT(num, payload, lenght);
        webSocket.broadcastTXT(payload, lenght);
        break;
        
      case WStype_BIN:
     
        hexdump(payload, lenght);

        // echo data back to browser
        webSocket.sendBIN(num, payload, lenght);
        break;
    }

}


void UpdateNTP() {

  //Connect to time server and get time
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
}

void setup() {

  
  pinMode(boiler, OUTPUT); //set boiler pin as output
  Serial.begin(115200);                       // Begin serial communication

   if ( !bmp.begin() ) {                      //check pressure sensor
    Serial.println("BMP180 KO!");
    while(1);
  } else {
    Serial.println("BMP180 OK");
  }
  
    // Connect to WiFi
  IPAddress ip(192, 168, 0, 20);              // set desired IP Address
  IPAddress gateway(192, 168, 0, 1);          // set gateway to match network
  IPAddress subnet(255, 255, 255, 0);         // set sbunet mask
  WiFi.config(ip, gateway, subnet);           // apply wifi settings

  WiFi.begin ( ssid, password );              // make connection
  
  while (WiFi.status() != WL_CONNECTED) {     // Wait for connection
  delay(500);
  Serial.println("Waiting to connect…");
  }
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());             // Print the local IP to access the server
  UpdateNTP();                                // update time
  server.begin();                             // Start the webserver
  Serial.println("Server listening");   

  webSocket.begin();                          // Open WebSocket
  webSocket.onEvent(webSocketEvent);          // Handle WebSocket event
  SPIFFS.begin();


  //Serve up the server files from SPIFFS
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/thermostat.js", SPIFFS, "/thermostat.js");
  server.serveStatic("/styles.css", SPIFFS, "/styles.css");
  server.serveStatic("/images", SPIFFS, "/images");

  }


//function to mainly handle the timer
void boilerUpdate(){
 int tStart = startHour*60+startMin;          // Convert the times into integer values 
 int tCurrent = hour()*60+minute();
 int tEnd = endHour*60+endMin;

  if ((turnedOn == true))
  {
    if (timerEnable== true)
    { 
      if (heatOn == true)
      {
        if (tStart < tEnd)
        {
          if (tCurrent < tStart && tCurrent < tEnd){
            digitalWrite(boiler, LOW);
            heatOn = false;
          }
          else if(tCurrent > tStart && tCurrent > tEnd){
            digitalWrite(boiler, LOW);
            heatOn = false; 
          }
        }
        else  //if tStart > tEnd
        {
          if (tCurrent >tEnd &&tCurrent < tStart){
          digitalWrite(boiler, LOW);
            heatOn = false; 
          }
        }
      }
      else //if heatOn is false
      {
        if (tStart < tEnd)
        {
          if (tCurrent > tStart && tCurrent < tEnd)
          {
            compareTemps();
          }
        }
        else // if tStart > tEnd
        {
          if (tCurrent > tStart && tCurrent > tEnd) //second part may be superfluous
          {
            compareTemps();
          }
          else if (tCurrent < tStart && tCurrent < tEnd)
          {
            compareTemps();
          }
        }
      }
    }
    else //if timer not enabled
    {
      
    }
  }
}

void compareTemps(){
  Serial.print("Actual temp: " );
  Serial.println(t);
  Serial.print("Requested temp: " );
  Serial.println(tempInt);
  if (t<tempInt-0.5){
              digitalWrite(boiler, HIGH);
              heatOn = true;
            }
            else if (t>tempInt+0.5){
              digitalWrite(boiler, LOW);
              heatOn = false;
            }
}

void loop() {

      
  // Get sensor values and format for sending


  server.handleClient();
  webSocket.loop();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    
    boilerUpdate();
    if (heatOn == true){
      compareTemps();
    }
    float tsense = dht.readTemperature();
    if ( ( (tsense<100) && (tsense>1) ) ){
      t = tsense;
    }
    realTemp =(prefixT + t);

    
    float hsense = dht.readHumidity();
    if ( ( (hsense<100) && (hsense>1) ) ){
      h = hsense;
    }
    
    humidity=(prefixH + h);
    
    p = bmp.readPressure() / 100.0F;
    pressure=(prefixP + p);
  }
 

 
}







void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year()); 
  Serial.println(); 
}



void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime(){
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
