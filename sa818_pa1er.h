/*
 * This file includes the default information. 
 * Specific configuration is made in config.h
 * 
 */

#include <HardwareSerial.h>
//#include <SoftwareSerial.h>

#define vhf 1
#define uhf 2

 // Structure to send information to the display

 class pa1erDisplay
 {
  public:
      void init();        //initiate the display
      void set();         //Update display screen
      char call[10];      //User call in Display
      char freqSa818[10]; //Freq set in the SA818
      char freqNew[10];   //Freq to be set (only display if different from freq
      int SQ;             //Squels level
      int Vol;
      int rssi;           // rssi of rx
      int dir;            //Direction of the signal (dopler)
      bool freqOk;        // Freq accepted by SA
      bool sqOk;

  private:
      int i; 

 };


class pa1erSa818
{
  public:
    void init();              //Setup communication channel
    bool startCom();      // init communication with SA818
    bool setGroup();          //Set the freq etc
    bool setVolume(int Vol);  //set Volume 1-8
    int getRssi();            // Value from 0-255, unit 1db
    bool readSa( char expected[40]);  //internal to read a line back
    
    int bw;                   //0=12.5K; 1=25K
    int band;                 // VHF / UHF; Internal set after freq set to select device activation
    char freqTx[10];            //400-480; 134-174; format 145.2750
    char freqRx[10];
    int setMHz;
    int setKHz;               // use 4 digit after MHz in stead of 3 so khz-1
    char subAudioTx[5];        //4 digit, for CTCSS 1-38; CDCSS 3 digit with I or N
    char subAudioRx[5];
    int SQ;                   //Range 1=8; 0=listening

  private:
    bool freqOk;
    
};
