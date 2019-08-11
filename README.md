# PA1ER's SA818 controller



Copyright (c) 2019 by Erik-Jan Roggekamp 

Code is free to use for private useage 

Homepage: http://www.pa1er.nl

------

### Idea of the project

For a project of a Radio Direction Finder I needed to have a receiver for VHF and UHF. I did'nt want to use my normal radio's because it is hard to switchoff the transmitter. In my cabinet I had still some SA818 from www.nicerf.com. They are being controlled by a RS232 signal. So the first idea was to have a small ESP, like a WEMOS D1 to controll the devices. I also wanted to import the information from the RDF. Then, the most ESP's are hard to use because the do not have 2 UARTS. For my MMDVM DMR repeater (PI1DOR) I still had a Arduino Due on stock. THis device has 4 UARTS so goed device to use for the test.

In the future it will be possible that i move on for also using other devices. For now, use my ideas in the code also for other projects.



### Status

The current status is that the device is running with the code as is. But I go on also using other devices to controll the SA818 and connect a UART to the RDF.



### Hardware

THe switch betrween VHF and UHF is made via the enable options (pin 6) of the SA818. So, 2 devices are in parallel and only one is active. Selected by a digital output pin.  Another option is to use 2 serial ports to connect  them.

Option I have a small OLED display for which I use the *Adafruit_SSD1306* routines. Those files are included in the project but are not maintained here. THe oled display is connected via the I2c bus.



![PA1ER_SA818](/Users/erikjan/Documents/Arduino/esp/sa818_pa1er/PA1ER_SA818.png)

### Software

There we are.

The main routine is the initialization, and the loop. The subroutines are placed in the pa1er.cpp file. Then there are 2 header files.

The file SA818_pa1er.h is the internal file for creating the needed info. 

The config.h file is file to personalize if you want to use the files as they are.



### Usage

To controll the SA818 you have to use the monitor function of the Arduino tool. You can also connect a serial terminal to the first serial port.

There are a few commands. They contain one (uppercase) letter direct followed by information. Some examples:

​	**F438.0875** sets the freq to 438.0875 MHz

​	**V4** sets the volume to level 4 (1-8)

​	**R** displays the RSSI of the received signal.

More information will be showed wen connected to the device.



------

If you are using those files, please let me know and give me feedback to evaluate the code.



'73, 

Erik-Jan PA1ER