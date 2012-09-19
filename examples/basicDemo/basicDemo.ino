/*****************************************************************************
 *
 *  Project Name:       Sharp Memory Lcd Library
 *  File Name:          basicDemo.ino
 *  Author:             Adam Fabio
 *  Creation Date:      03/12/2012
 *
 *  Description:   		This file provides a basic demo of the 
 * 						funcitonality of the Sharp Memory Lcd Library. 
 *
 ******************************************************************************/
/* Hardware setup:  
This demo uses the LCD in harware vcom mode.  
Arduino Pin 3 is conncted to  ExtCom (LCD breakout Pin 5)
ExtMode (LCD Pin 7) is connected to 5V.
Sharp Memory Lcd Breakout  <-> Arduino Uno
Name   SLCD PIN    ARDU PIN
VCC 	   1		  5V
SCLK	   2		  12
SDATA      3		  11
SCS	       4  		  10
EXTCOM     5		   3
ENABLE     6		   4
ExtMode	   7		  5V
GND        8          GND
*/

//make sure you include SPI in any Sharp Memory LCD software
#include <SPI.h>
#include <MemoryLCD.h>

//Create the LCD using a hardware timer 
MemoryLCD memLcd(EXT_HARDWARE);

//Bitmap data for the cat - used below
PROGMEM prog_uchar catBmp[128] = 
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x20,
    0x02, 0x08, 0x08, 0x40,
    0x01, 0x14, 0x14, 0x80,
    0x00, 0x92, 0x24, 0x00,
    0x60, 0x22, 0x22, 0x06,
    0x18, 0x21, 0x42, 0x18,
    0x06, 0x41, 0xC1, 0x60,
    0x00, 0x40, 0x01, 0x00,
    0x00, 0x44, 0x11, 0x00,
    0xF0, 0x44, 0x11, 0x1E,
    0x00, 0x44, 0x11, 0x00,
    0x00, 0x40, 0x01, 0x00,
    0x00, 0x5C, 0x9D, 0x00,
    0x00, 0x20, 0x02, 0x00,
    0x00, 0x10, 0x04, 0x00,
    0x00, 0x0E, 0x38, 0x40,
    0x00, 0x02, 0x20, 0xA0,
    0x00, 0x04, 0x11, 0x20,
    0x00, 0x08, 0x0A, 0x40,
    0x00, 0x10, 0x04, 0x80,
    0x00, 0xD0, 0x05, 0x80,
    0x01, 0x30, 0x86, 0x40,
    0x02, 0x18, 0x8C, 0x20,
    0x02, 0x08, 0x88, 0x20,
    0x03, 0x88, 0x88, 0xE0,
    0x06, 0x09, 0xC8, 0x30,
    0x07, 0xFF, 0x7F, 0xF0,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00 
};

void setup()
{
  //start the LCD libarary
  memLcd.begin();
}

void loop()
{

    //Display an intro to the Library
    //Place "The" at pixel 0,30 (top center)
    memLcd.putString(0,30, "The");
    memLcd.putString(15,10, "Rengineer");
    memLcd.putString(30,25,"Sharp");
    memLcd.putString(45,18,"Memory");
    memLcd.putString(60,2,"LCD Library");
    //Push the text to the LCD
    memLcd.refresh();
    //delay the text can be seen
    delay(4000); 
    
    //clear the LCD
    memLcd.clear();
    memLcd.putString(0,5, "Plot Circles");
    //Plot a circle of radius 20 at pixel 42,42.
    memLcd.Circle(42,42,20,0);
    //Plot a circle of radius 20 at pixel 60,20
    memLcd.Circle(60,20,20,0);
    //Push circles and text to the LCD
    memLcd.refresh();
    delay(4000); 
    
    memLcd.clear();
    memLcd.putString(0,10, "Plot Lines");
    memLcd.PlotLine(0,0,95,95,0);
    memLcd.PlotLine(0,95,95,0,0);
    memLcd.refresh();
    delay(4000); 
    
    memLcd.clear();
    memLcd.putString(0,27, "Draw");
    memLcd.putString(15,20, "Bitmaps");
    //Draw catBmp - (defined above)
    //Place it at pixel 40,32. 
    //catBmp is 32x32 pixels.  
    memLcd.printBitmap(40,32,catBmp,32,32);
    memLcd.refresh();
    delay(4000);
    //clear the LCD in prep for the next loop
    memLcd.clear();
}
