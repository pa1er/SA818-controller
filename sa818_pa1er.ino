/*
 * This programm is mend to communicate with the SA818 TRX modules
 * It will also being connected to a Radio Direction Finder to get a direction of the signal
 * Configurations are made in the config.h file
 * 
 * Realtime commands are given by the serial port over USB
 * 
 * More information on http://www.pa1er.nl
 * 
 * Use this routines while leaving my callsign and name in the code
 * 
 * Erik-Jan Roggekamp, PA1ER
 * 
 */

#include <WiFi.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "sa818_pa1er.h"
#include "config.h"


String cmd;


// ejr: Bord Arduino Due:


//#define OLED_RESET 4 // not used
//Adafruit_SSD1306 display(OLED_RESET);  staat in de pa1er module...

//HardwareSerial serialSa818(1);     // Define serial pins in configh
//HardwareSerial serialDopler(1);    // Later gebruik voor input dopler

pa1erDisplay pa1erDisplay;
pa1erSa818 pa1erSa818;



void setup() {
  // put your setup code here, to run once:
  char s[32];
  
  Serial.begin(115200);
  Serial.println(F("SA818 control PA1ER"));
  Serial.println(F("======================================================"));
  Serial.println(F("Commands are:"));
  Serial.println(F("F145.275  = set FREQ to 145.275"));
  Serial.println(F("B125      = set Bandwith to 12.5 KHz Option: 125 or 25"));
  Serial.println(F("V4        = set volume to 4, Value 1-8"));
  Serial.println(F("S4        = set Squels to 4, Value 1-8"));
  Serial.println(F("R         = Read RSSI"));
  Serial.println(F("======================================================"));

  #ifdef debug
    Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
  #endif
  
  pa1erDisplay.init();
  pa1erSa818.init();  

}

void loop() {
  // put your main code here, to run repeatedly:
  String resp;
  String strMHz;
  String strKHz;
  String t;
  char s[32];
  bool stat;
  String readString;


// Check input from serial line - PC
  while(Serial.available()) {
  resp = Serial.readString();// read the incoming data as string
  Serial.print("Command: ");
  Serial.println(resp); 
  cmd = resp.substring(0,2);

  switch (cmd[0]) {
    case 'F':
      pa1erSa818.setMHz = resp.substring(1,4).toInt();
      pa1erSa818.setKHz = resp.substring(5,10).toInt();
      #ifdef debug
      Serial.println(F("Command freq"));
      sprintf(s,"freq: %d-%d",pa1erSa818.setMHz,pa1erSa818.setKHz);
      Serial.println(s);
      #endif
      stat = pa1erSa818.setGroup();  // Do something with the status?
      break;
    case 'B':
      #ifdef debug
      Serial.println(F("Command BW"));
      #endif
      sprintf(s,"%c",cmd[1]);
      pa1erSa818.bw = atoi(s);
      pa1erSa818.setGroup();
      break;      
    case 'V':
      #ifdef debug
      Serial.println(F("Command Volume"));
      #endif
      sprintf(s,"%c",cmd[1]);
      stat= pa1erSa818.setVolume(atoi(s));
      break;
    case 'S':
      #ifdef debug
      Serial.println(F("Command Squels"));
      #endif
      sprintf(s,"%c",cmd[1]);
      pa1erSa818.SQ = atoi(s);
      pa1erSa818.setGroup();
      pa1erDisplay.set();
      break;
    case 'R':
      #ifdef debug
      Serial.println(F("Command RSSI"));
      #endif
      sprintf(s, "RSSI: %d",pa1erSa818.getRssi());
      Serial.println(s);
      break;
    default:
      Serial.println(F("Unknown command"));
      break;

      
  } // case
  }//while input op terminal


// Update the display
  pa1erDisplay.rssi = pa1erSa818.getRssi();
  pa1erDisplay.set();


// Check if the SA will tell us something while not listning

 while (Serial1.available()) 
 {
   delay(2);  //delay to allow byte to arrive in input buffer
   char c = Serial1.read();
   readString += c;
 }
  #ifdef debug
 if (readString.length() >0) {
   Serial.print(F("In Main loop"));
   Serial.println(readString);
      readString="";
 } 
 #endif
} // loop
