/*
 * This routines are mend to helps the mail programm of communicating with the SA818 TRX modules
 * Use this routines while leaving my callsign and name in the code
 * 
 * Erik-Jan Roggekamp, PA1ER
 * 
 */

#include "sa818_pa1er.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <HardwareSerial.h>
#include <wire.h>

#include "config.h"

#define OLED_RESET 4 // not used
Adafruit_SSD1306 display(OLED_RESET);
//HardwareSerial serialSa818(1);     // Define serial pins in configh // use defaults pins, config not used

//SoftwareSerial serialSa818( D7,D8 ); 
//HardwareSerial serialDopler(2);    // Later gebruik voor input dopler

pa1erSa818 pa1er;
pa1erDisplay pa1erDisp;
char s[32];
String resp;


/*
 *          Initialization of the display params
 */
void pa1erDisplay::init()
{
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3c, 0, 21, 20, 800000);   ////  let op de pinnummer die nog moeten komen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c, 0); // Default pin number, definition not needed address is 078
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(1,0);
  display.println(" PA1ER");
  display.display(); 

  sprintf(pa1erDisp.call, "%s", Call);
  #ifdef debug
  Serial.println(F("Display init done"));
  #endif
  pa1erDisp.rssi=4;
//  pa1erDisp.Vol=8;

}

/*
 *            This routine will set the display under normal conditions (not during the change of params) 
 */
void pa1erDisplay::set()
{

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(32,0);
  sprintf(s, "%s", Call);
  display.println(s);
  display.setTextSize(1);
  display.setCursor(0,16);
  display.println("Freq:");
  display.setTextSize(2);
  display.setCursor(30,16);
  if (pa1erDisp.freqOk == true){
     display.println(pa1erDisp.freqSa818);
  } 
  else{
     display.println("not set");
  }
  display.setTextSize(1);
  display.setCursor(0,40);
  display.println("SQL :");  
  display.setCursor(30,40);
  display.println(pa1erDisp.SQ);
  display.setCursor(60,40);
  display.println("VOL:");  
  display.setCursor(90,40);
  display.println(pa1erDisp.Vol); 
  display.setCursor(0,50);
  display.println("RSSI: ");
  display.setCursor(30,50);
  if (pa1erDisp.freqOk == true) { display.println(rssi); }
  display.setCursor(60,50);
  display.println("DIR: ");
  display.setCursor(90,50);
  display.println(pa1erDisp.dir);

    
  display.display(); 
}


/*
 *            Init of the values of the SA818
 */

void pa1erSa818::init()
{
  Serial1.begin(9600);
  bw = 1;
  setMHz=145;
  setKHz=2750;
  SQ=4;
  setGroup();
  setVolume(6);
  pa1erDisp.SQ = SQ;
  pa1erDisp.sqOk = false;
  pa1erDisp.freqOk = false;
}


/*
 *            Starting the communications with the SA818, Also needed after change VHF/UHF (is other module)
 */
bool pa1erStartCom()
  {

  String readString;
  int count; 
  char c[40];
  
  sprintf (s, "AT+DMOCONNECT\r\n");
  Serial1.println(s);
  #ifdef debug
  Serial.print(F("send: "));
  Serial.println(s);
  #endif
  delay(700);

  count =0;
  while ( count < 5) // Sometime have to do it a number of times
  {
    
    sprintf(c,"+DMOCONNECT:0");
    if (pa1er.readSa(c) == true) {
      break; // connected"
    }
    count++;
    if (count == 5){
      return false;
    }
  Serial1.println(s);
  #ifdef debug
  Serial.print(F("send: "));
  Serial.println(s);
  #endif
  delay(500);
    
  }
}

/*
 *              Set the RX/TX freq, Bandwith, Squels and CTCSS. Lastone is not used for now 
 */
bool pa1erSa818::setGroup()
{
   String readString;
   int count;
   char c[40];
   
  // check params, then set SA818
   #ifdef debug
   Serial.println(F("setGroup."));
   #endif
   if (bw !=1)  // Bandwidh can only be 1(25KHz) or 0 (12.5 KHz) 
   {
    bw = 0;
    }
   // Don't use Subaudio at this moment, to be used later
   sprintf(subAudioTx, "0000");
   sprintf(subAudioRx, "0000");

   freqOk = false;
   if (setMHz > 133 and setMHz < 175) // VHF
   {
    freqOk=true;
    if (band != vhf)
    {                                 // Here we have to activate VHF module
      #ifdef debug
      Serial.println(F("VHF."));
      #endif
      pa1erDisp.freqOk = false;
      digitalWrite(pinUhf, LOW);      // Deactivate UHF madule
      digitalWrite(pinVhf, HIGH);     //activate VHF modiel
      delay(10);
      
      if (pa1erStartCom() == false)
      {
        Serial.println(F("Starting communication VHF fails"));
        return false;         
      }
      band = vhf;
    }
    
    
   }
   if (setMHz > 399 and setMHz < 481) // UHF
   {
    freqOk=true;
    if (band != uhf)
    {   // Here we have to activate UHF module
      #ifdef debug
      Serial.println(F("UHF."));
      #endif
      pa1erDisp.freqOk = false;
      digitalWrite(pinVhf, LOW);      // Deactivate VHF madule
      digitalWrite(pinUhf, HIGH);     //activate UHF modiel
      delay(10);
      
      if (pa1erStartCom() == false)
      {
        Serial.println(F("Starting communication UHF fails"));
        return false;
      }
      band = uhf;
    }
   }

   if (freqOk == false)
   {
    sprintf(s, "Freq out of range: %03d.%04d", setMHz, setKHz);
    Serial.println(s);
    Serial.println(F("Range VHF: 134.0000 - 174.0000"));
    Serial.println(F("Range UHF: 400-0000 - 480.0000"));
    return false;
   }
   sprintf(freqRx, "%03.3d.%04.4d",setMHz, setKHz);
   sprintf(freqTx, "%03.3d.%04.4d",setMHz, setKHz);            // only work with RX now, so no reason for other TX freq 

   if (SQ<0){
    SQ = 0;
   }
   if (SQ>8){
    SQ = 8;
   }

// Now we are able to crate the string to be send to the SA818
// Format：AT+DMOSETGROUP=BW，TX_F，RX_F，Tx_subaudio，SQ，Rx_subaudio

  sprintf (s, "AT+DMOSETGROUP=%i,%s,%s,%s,%i, %s\r\n", bw, freqTx,freqRx,subAudioTx,SQ,subAudioRx);
  Serial1.println(s);
  #ifdef debug
  Serial.print(F("send: "));
  Serial.println(s);
  #endif
  delay(700);

  count =0;
  while ( count < 5) // Moeten soms een paar keer proberen om communicatie op te zetten
  {
    
    sprintf(c,"+DMOSETGROUP:1");
    if (pa1er.readSa(c) == true) {
      Serial.println(F("Freq set"));
      sprintf(pa1erDisp.freqSa818, "%s",freqRx);
      pa1erDisp.freqOk = true;
      pa1erDisp.SQ = SQ;
      break; // connected"
    }
    count++;
    if (count == 5){
      return false;
    }
//  Serial1.println(s);
  #ifdef debug
  Serial.print(F("send: "));
  Serial.println(s);
  #endif
  delay(500);
    
  }
}


/*
 *            We can set the Valume of the active module
 */


bool pa1erSa818::setVolume(int Vol)
{
   if (Vol < 0){
    Vol = 0;
   }
   if (Vol > 8){
    Vol = 8;
   }
  sprintf (s, "AT+DMOSETVOLUME=%i\r\n", Vol);
  Serial1.println(s);
  #ifdef debug
  Serial.println(s);
  #endif
  pa1erDisp.Vol=Vol;
  pa1erDisp.set();

  delay(500);

  while(Serial1.available()) {
  resp = Serial1.readString();// read the incoming data as string
  #ifdef debug
  Serial.print("responce SA818 volume: ");
  Serial.println(resp); 
  #endif
 
   return true;  // No real check but is that needed?
  }
  
}

/*
 *            This routine will reads the RSSI from the SA818
 */

int pa1erSa818::getRssi()
{
  String readString;
  readString="";
  
  sprintf (s, "RSSI?\r\n");
  Serial1.println(s);


  delay(1000);

  while (Serial1.available()) 
   {
   delay(5);  //delay to allow byte to arrive in input buffer
   char c = Serial1.read();
   if ( (c != '\r') and ( c != '\n')) {
     readString += c;
     }
   }
 if (readString.length() >0) {
   #ifdef debug
   Serial.print("RSSI: ");
   Serial.println(readString);
   #endif
   return readString.substring(5).toInt();
 }
}



/*
 *            This routine is internal to read the serial responce of the SA818 and checks the expected responce
 */
bool pa1erSa818::readSa( char expected[40])
{
  String readString;

  delay(50);

while (Serial1.available()) 
 {
   delay(5);  //delay to allow byte to arrive in input buffer
   char c = Serial1.read();
   if ( (c != '\r') and ( c != '\n')) {
//   Serial.println(c);
     readString += c;
   }
 }
 if (readString.length() >0) {
  if (readString.equals(expected)) {
   #ifdef debug
   Serial.print("Accepted: ");
   Serial.println(readString);
   #endif 
   readString="";
   return true;
    }// end right answer
 
  }  //end there are characters but wrong?
   #ifdef debug
   Serial.print("Not accepted: ");
   Serial.println(readString);
   #endif
 readString="";
// ff niet voor de test  ;  return false; // Geen antwoord? Kan bijna niet
  return false;

}
