//TheRengineer Sharp Memory Lcd Library - 
//Cat Demo "kitty"

//make sure you include SPI in any Sharp Memory LCD software
#include <SPI.h>
#include <MemoryLCD.h>
#include "catBmps.h"
//Create the LCD using a hardware timer 
MemoryLCD memLcd(EXT_HARDWARE);



void setup()
{
  //start the LCD libarary
  memLcd.begin();
}

void loop()
{
  unsigned char whichBmp=0;
  int i,j;
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



  memLcd.clear();
  memLcd.putString(0,27, "Draw");
  memLcd.putString(15,20, "Bitmaps");
  //Draw catBmp - (defined above)
  //Place it at pixel 40,32. 
  //catBmp is 32x32 pixels.  

  while(1)
  {
    //run the top right to left 
    for(j=96;j>=0;j-=8)
    {
      memLcd.clear();  
      if(whichBmp & 1)
      {
        memLcd.printBitmap(0,j,left1,32,32);
      }
      else
      {
        memLcd.printBitmap(0,j,left2,32,32);
      }
      whichBmp++;
      memLcd.refresh();
      delay(150); 
    }
    //run down the left side
    for(i=0;i<=64;i+=8)
    {
      memLcd.clear();  
      if(whichBmp & 1)
      {
        memLcd.printBitmap(i,0,down1,32,32);
      }
      else
      {
        memLcd.printBitmap(i,0,down2,32,32);
      }
      whichBmp++;
      memLcd.refresh();
      delay(150); 
    }
    //run the bottom left to right 
    for(j=0;j<=64;j+=8)
    {
      memLcd.clear();  
      if(whichBmp & 1)
      {
        memLcd.printBitmap(64,j,right1,32,32);
      }
      else
      {
        memLcd.printBitmap(64,j,right2,32,32);
      }
      whichBmp++;
      memLcd.refresh();
      delay(150); 
    }
    //run to the center diagonally up + left
    for(j=64;j>=32;j-=8)
    {
      memLcd.clear();  
      if(whichBmp & 1)
      {
        memLcd.printBitmap(j,j,upLeft1,32,32);
      }
      else
      {
        memLcd.printBitmap(j,j,upLeft2,32,32);
      }
      whichBmp++;
      memLcd.refresh();
      delay(150); 
    }
    //sit there and look cool
    memLcd.printBitmap(32,32,yawn2,32,32);
    memLcd.refresh();
    delay(500);
    //yawn
    memLcd.printBitmap(32,32,yawn3,32,32);
    memLcd.refresh();
    delay(500);
    //sit there and look cool
    memLcd.printBitmap(32,32,yawn2,32,32);
    memLcd.refresh();
    delay(500);
    //sleep for awhile
    for(i=0;i<=6;i++)
    {
      if(whichBmp & 1)
      {
        memLcd.printBitmap(32,32,sleep1,32,32);
      }
      else
      {
        memLcd.printBitmap(32,32,sleep2,32,32);
      }
      whichBmp++;
      memLcd.refresh();
      delay(500); 
    }
    //wake up
    memLcd.printBitmap(32,32,awake,32,32);
    memLcd.refresh();
    delay(500);
    //run to the upper right diagonally up + right
    i=40;
    for(j=32;j<=64;j+=8)
    {
      i-=8;
      memLcd.clear();  
      if(whichBmp & 1)
      {
        memLcd.printBitmap(i,j,upRight1,32,32);
      }
      else
      {
        memLcd.printBitmap(i,j,upRight2,32,32);
      }
      whichBmp++;
      memLcd.refresh();
      delay(150); 
    }
  }
}

