/*****************************************************************************
 *
 *  Project Name:       Sharp Memory Lcd Library
 *  File Name:          MemoryLcd.cpp
 *  Author:             Adam Fabio
 *  Creation Date:      03/12/2012
 *
 *  Description:   		This library provides an interface to the 
 * 						sharp memory lcd. 
 *
 ******************************************************************************/
#include "Arduino.h"
#include "spi.h"


#include "MemoryLCD.h"
#include "stenFont.h"



/*Array of reversed bytes.  
This array is used to reverse bytes sent to the sharp memory lcd.  For speed
it is kept in memory.  
Future TODO - test placement into program memory / function call use.  Will cause
a speed hit, but that may not have an impact if comms with the LCD is still the 
longest delay.
*/
unsigned char   revByte[256] = {0x0,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,
0x50,0xd0,0x30,0xb0,0x70,0xf0,0x8,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,0x18,0x98,
0x58,0xd8,0x38,0xb8,0x78,0xf8,0x4,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,0x14,0x94,
0x54,0xd4,0x34,0xb4,0x74,0xf4,0xc,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,0x1c,0x9c,
0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,0x2,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,0x12,0x92,
0x52,0xd2,0x32,0xb2,0x72,0xf2,0xa,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,0x1a,0x9a,
0x5a,0xda,0x3a,0xba,0x7a,0xfa,0x6,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,0x16,0x96,
0x56,0xd6,0x36,0xb6,0x76,0xf6,0xe,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,0x1e,0x9e,
0x5e,0xde,0x3e,0xbe,0x7e,0xfe,0x1,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,0x11,0x91,
0x51,0xd1,0x31,0xb1,0x71,0xf1,0x9,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,0x19,0x99,
0x59,0xd9,0x39,0xb9,0x79,0xf9,0x5,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,0x15,0x95,
0x55,0xd5,0x35,0xb5,0x75,0xf5,0xd,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,0x1d,0x9d,
0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,0x3,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,0x13,0x93,
0x53,0xd3,0x33,0xb3,0x73,0xf3,0xb,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,0x1b,0x9b,
0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,0x7,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,0x17,0x97,
0x57,0xd7,0x37,0xb7,0x77,0xf7,0xf,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,0x1f,0x9f,
0x5f,0xdf,0x3f,0xbf,0x7f,0xFF};








/*
*Add a basic font (orbittron for now)
*Func to add a font to the font list
*addFont(char *name,fwidth,fheight,descriptor pointer,bitmap pointer)
*/
unsigned char MemoryLCD::addFont(char *name,FONT_CHAR_INFO *fontInfo,unsigned char *fontBmap)
{
	unsigned char retVal = 0;
	//make sure we have room in the system for another font
	if(this->curFontCount < MAX_NUM_FONTS)
	{
		//verify we have good pointers
		if((name != NULL)&&(fontInfo!=NULL)&&(fontBmap!=NULL))
		{
			strcpy(fontList2[curFontCount].fontName,name);
			fontList2[curFontCount].fontInfo = fontInfo;
			fontList2[curFontCount].fontBmap = fontBmap;
			this->curFontCount++;
			retVal = 1;
		}
	}
	return retVal;
}




/*
*  constructor - set everything up
*/
MemoryLCD::MemoryLCD(unsigned char inMode)
{
    unsigned char lcdtemp;
	extMode = inMode;
	
	if(extMode == EXT_HARDWARE)
    {
		//For hardware timer
		pinMode(3, OUTPUT);
		analogWrite(3,127);
		TCCR2B = TCCR2B & 0xFF;
		//end hardware timer
	}
	else
	{
		cur_vBit = 0;
		this->vbitIntervalMs = 750;
	}
	curFontCount=0;
	this->curFont = NULL;

	lcdtemp=addFont("orbit",orbitron10ptDescriptors,orbitron10ptBitmaps);
	this->curFont = NULL;
	lcdtemp = setFont("orbit");
	memset(lcdShadow,0,sizeof(lcdShadow)); 
	pinMode(SLAVE_SELECT_PIN, OUTPUT);
	pinMode(DISP_ENABLE, OUTPUT);
	// initialize SPI:
    SPI.setBitOrder(LSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV32);
}

//nothing to destruct		
MemoryLCD::~MemoryLCD()
{
	;	
}

void MemoryLCD::begin()
{
    SPI.begin(); 
	//Clear the lcd shadow memory (and issue the clear command)
	this->enable();
	this->clear();
}


/*
*Switch the VCOM bit in software - needed for the Sharp Memory LCD. 
*If we're using hardware VCOM switching, this function just falls through
*/
void MemoryLCD::handleVbit(char *cmd)
{
	if(extMode != EXT_HARDWARE)
	{
		if(millis() - this->prev_ms >= this->vbitIntervalMs) 
		{
			//reset the timer
			this->prev_ms = millis();
			if(cur_vBit == 0)
			{
			  cur_vBit = 1;
			  *cmd = *cmd | V_HIGH_BIT;
			}
			else
			{
			  cur_vBit = 0;
			  *cmd = *cmd & V_BIT_MASK;
			}
		}
	}
}
/*
*  Place a font character into the LCD buffer. 
*
*/

void MemoryLCD::RomPutFontchar(int i, int j,  unsigned char asciiChar)
{
	unsigned char thisFontByte;
	unsigned char lastFontByte;
	unsigned char thisMaskByte;
	unsigned char offset;
	unsigned char *fontPtr;
	unsigned int iIndex = 0;
	unsigned char jByte = j/8;
	unsigned char jByteCount;
	unsigned char jByteIndex;
	FONT_CHAR_INFO curFontInfo;
	FONT_CHAR_INFO *fontInfoRomPtr;

	asciiChar -=32; //convert ascii value to fontinfo array index	
	fontInfoRomPtr=curFont->fontInfo;
	fontInfoRomPtr +=asciiChar;
	memcpy_P(&curFontInfo, fontInfoRomPtr, sizeof(FONT_CHAR_INFO));

	fontPtr = curFont->fontBmap + (curFontInfo.offset);
	for(iIndex = 0; iIndex < curFontInfo.fHeight; iIndex++)
	{
		//Find our offset from the last byte boundary
		offset = j%8;
		//Find how many bytes we're going to need in the j dimension (uses integer divide with round up)
		jByteCount = curFontInfo.fWidth / 8; 
		//if the font width pushes beyond a byte boundary, bump the number of bytes needed.
		if( curFontInfo.fWidth % 8 != 0)
		{
			jByteCount++;
		}
		//if we are starting on a byte boundary, use this simple case. 
		if(offset == 0)
		{
			for(jByteIndex=0;jByteIndex < jByteCount;jByteIndex++)
			{
				thisFontByte = pgm_read_byte_near(fontPtr+(iIndex*jByteCount)+jByteIndex);
				thisFontByte = ~thisFontByte;
				if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
				{
					if((jByte+jByteIndex>=0) && (jByte+jByteIndex < LCD_COL_BYTES))
					{								
						lcdShadow[i+iIndex][jByte+jByteIndex] = revByte[thisFontByte];
					}
				}
			}
			
		}
		else  //Not starting on a byte boundary - more complicated.
		{
			//first byte is special.  do it outside the loop
			thisFontByte = pgm_read_byte_near(fontPtr+iIndex*jByteCount);
			thisFontByte = thisFontByte >> offset;  //move the first byte over
			thisMaskByte = (unsigned char)0xFF >> offset;   		//build a mask;
			if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
			{
				if((jByte>=0) && (jByte < LCD_COL_BYTES))
				{				
					lcdShadow[i+iIndex][jByte] |= revByte[thisMaskByte];
					//swap colors to match sharp's color scheme
					thisFontByte = ~thisFontByte;
					//Or in the inverted font (reverse for bit order).  
					//set the bits in this byte
					lcdShadow[i+iIndex][jByte] &= revByte[thisFontByte];  
				}
			}
			for(jByteIndex=1;jByteIndex < jByteCount;jByteIndex++)
			{				
				//first deal with pixels left over from the last character
				//set up the bits in the first font byte
				lastFontByte = pgm_read_byte_near(fontPtr+(iIndex*jByteCount)+(jByteIndex-1));
				lastFontByte = lastFontByte << (8 - offset);  
								
				//set up the bits in the second font byte (shift and mask)
				thisFontByte = pgm_read_byte_near(fontPtr+(iIndex*jByteCount)+jByteIndex);
				thisFontByte = thisFontByte >> offset; 

				//now put it all together
				thisFontByte = thisFontByte | lastFontByte;
				thisFontByte = ~thisFontByte;
				//write it to the shadow (destroy any extra bits to the right)
				if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
				{
					if((jByte+jByteIndex>=0) && (jByte+jByteIndex < LCD_COL_BYTES))
					{							
						lcdShadow[i+iIndex][jByte+jByteIndex] = revByte[thisFontByte];
					}
				}
				//first deal with pixels left over from the last character
			}
			//Last Byte is special - do this outside the loop
			//set up the bits in the second font byte
			thisFontByte = pgm_read_byte_near(fontPtr+(iIndex*jByteCount)+jByteCount-1);
			thisFontByte = thisFontByte << (8 - offset);  
			
			thisFontByte = ~thisFontByte;
			if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
			{
				if((jByte+jByteCount>=0) && (jByte+jByteCount < LCD_COL_BYTES))
				{							
					lcdShadow[i+iIndex][jByte+jByteCount] = revByte[thisFontByte]; //use the mask
				}
			}
		}
	}
}

/*
* Disable the lcd
*/
void MemoryLCD::enable()
{

	digitalWrite(DISP_ENABLE, HIGH);   // Display enabled (shows lcd memory)
	//start external vcom
	if(extMode == EXT_HARDWARE)
    {
		analogWrite(3,127);
	}
}

/*
*Enable the lcd
*/
void MemoryLCD::disable()
{
	//stop the external vcom 
	if(extMode == EXT_HARDWARE)
    {
		analogWrite(3,0);
	}
	digitalWrite(DISP_ENABLE, LOW);   // Display Disabled (all cloudy)
}

/*
* Clear the LCD and buffer 
* sets the buffer to 0, sends the clear command
* to the lcd
*/
void MemoryLCD::clear()
{
	memset(lcdShadow,0xFF,sizeof(lcdShadow));  //first clear shadow ram
	//now clear the lcd
	digitalWrite(SLAVE_SELECT_PIN, HIGH);	
	delay(LCD_DELAY);
	SPI.transfer(CMD_CLEAR); //send command
	SPI.transfer(0); //send trailer
	delay(LCD_DELAY);
	digitalWrite(SLAVE_SELECT_PIN, LOW); 
}

/*
* Send the buffer to the lcd
* Any changes made since the last refresh will appear
* On screen when this function is run.
*/
void MemoryLCD::refresh()
{
	char cmdByte = 0;
	int i = 0;
	int j = 0;
	//set the command byte
	cmdByte=CMD_WRITE_ROW;
	//Handle the polarity switch needed by the LCD
	handleVbit(&cmdByte);
	//setup for the SPI write
	digitalWrite(SLAVE_SELECT_PIN, HIGH);	
	delay(LCD_DELAY);
	SPI.transfer(cmdByte);
	for(i = 0; i < LCD_ROWS; i++)
	{
		SPI.transfer(i+1);
		for(j = 0; j < LCD_COL_BYTES; j++)
		{
			SPI.transfer(lcdShadow[i][j]);
		}
		//send trailer
		SPI.transfer(0);
	}
	//send final trailer
	SPI.transfer(0);
	//cleanup after the SPI write
	delay(LCD_DELAY);
	digitalWrite(SLAVE_SELECT_PIN, LOW);	
}

/*
* Swap all the pixel colors on the LCD - 
* Cloudy becomes clear, clear becomes cloudy.
*/
void MemoryLCD::invertColors()
{
	int i,j;
	for(i=0;i<LCD_ROWS;i++)
	{
		for(j=0;j<LCD_COL_BYTES;j++)
		{
			lcdShadow[i][j]=~lcdShadow[i][j];
		}
	}
	this->refresh();
}

/* Set a pixel in the buffer (and possibly on the screen)
* i,j: pixel location
* color: pixel color (0 = cloud, 1 = shiney)
* sendnow: send the pixel to the lcd now? 1=YES, 0=NO
* NOTE: due to the design of the sharp memory lcd, 
* sending the pixel caues the entire row to be sent.
*/
void MemoryLCD::setPixel(int i,int j,unsigned char color,unsigned char sendNow)
{
	unsigned char rowByte = j / 8;
	unsigned char rowBit = j % 8;
	unsigned char mask = 0x01 << (rowBit);
	if((i<LCD_ROWS) || (j < LCD_COLS))
	{
		if(color)
		{

		lcdShadow[i][rowByte] = lcdShadow[i][rowByte] | mask;
		
		}
		else
		{
			mask = ~mask;
			lcdShadow[i][rowByte] = lcdShadow[i][rowByte] & mask;
		}
		if(sendNow)
		{
			sendRow(i);
		}
	}
}


/*
* Send a row of pixels to the LCD
*/
void MemoryLCD::sendRow(int row)
{
	char cmdByte = 0;
	int j = 0;
	cmdByte=CMD_WRITE_ROW;
	//swap the vbit (if needed)
	handleVbit(&cmdByte);
	//set up the SPI transfer
	digitalWrite(SLAVE_SELECT_PIN, HIGH);	
	SPI.transfer(cmdByte);
	//Send the row count to the LCD (add 1 because lcd counts first row as 1)
	SPI.transfer(row+1);
	for(j = 0; j < LCD_COL_BYTES; j++)
	{
		SPI.transfer(lcdShadow[row][j]);
	}
	//send trailer
	SPI.transfer(0);
	SPI.transfer(0);
	//clean up after SPI transfer
	digitalWrite(SLAVE_SELECT_PIN, LOW);	
}

/*
*  Bresenham's line algorithm.  
*  Draw a line from i0,j0 to i1,j1 with color given. 
*  Attributes here to Wikipedia
*/ 
void  MemoryLCD::PlotLine(int i0, int j0, int i1, int j1,unsigned char color)
{
	int dx = abs(i1-i0);
	int dy = abs(j1-j0);
	int sx,sy,err,e2;
	if (i0 < i1) 
		sx = 1;
	else 
		sx = -1;
    if (j0 < j1)
		sy = 1; 
	else 
		sy = -1;
    err = dx-dy;
 
    while(1)
	{
		setPixel(i0,j0,0,color);
		if( i0 == i1 and j0 == j1)
			break;	 //exit loop
		e2 = 2*err;
		if (e2 > -dy) 
		{		
			err = err - dy;
			i0 = i0 + sx;
		}
		if(e2 <  dx)
		{
			err = err + dx;
			j0 = j0 + sy;
		}
    }
}

/*
*	Midpoint circle algorithm 
*	Draw a circle at i_start, j_start, with radius, and color as given
*   Attributions to wikipedia here. 
*/	
void MemoryLCD::Circle(int i_start, int j_start, int radius, unsigned char color)
{
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int i = 0;
  int j = radius;
 
  setPixel(i_start, j_start + radius,0,color);
  setPixel(i_start, j_start - radius,0,color);
  setPixel(i_start + radius, j_start,0,color);
  setPixel(i_start - radius, j_start,0,color);
 
  while(i < j)
  {
    if(f >= 0) 
    {
      j--;
      ddF_y += 2;
      f += ddF_y;
    }
    i++;
    ddF_x += 2;
    f += ddF_x;    
    setPixel(i_start + i, j_start + j,0,color);
    setPixel(i_start - i, j_start + j,0,color);
    setPixel(i_start + i, j_start - j,0,color);
    setPixel(i_start - i, j_start - j,0,color);
    setPixel(i_start + j, j_start + i,0,color);
    setPixel(i_start - j, j_start + i,0,color);
    setPixel(i_start + j, j_start - i,0,color);
    setPixel(i_start - j, j_start - i,0,color);
  }
}



/*  
*   Check if a font is available in the system
*	Scans the font list for the name given in the arguments
*   Returns 1 on success, 0 on failure
*/
unsigned char MemoryLCD::checkFont(char *lkupFont)
{
	int idx;
	unsigned char _retVal = 0;
	for(idx=0; idx<curFontCount; idx++)
	{
		if(strcmp(lkupFont,fontList2[idx].fontName) == 0)
		{
			_retVal = 1;
			break;
		}
	}
	return _retVal;
}

/*  
*   Set the currently active font
*	Scans the font list for the name given in the arguments
*   If the name is found, that font is set as the current font
*   Returns 1 on success, 0 on failure
*/
unsigned char MemoryLCD::setFont(char *lkupFont)
{
	int idx;
	unsigned char _retVal = 0;
	for(idx=0; idx<curFontCount; idx++)
	{
		if(strcmp(lkupFont,fontList2[idx].fontName) == 0)
		{
			_retVal = 1;
			this->curFont = &(fontList2[idx]);
			break;
		}
	}
	return _retVal;
}


/* 
*  Copy a string to the LCD buffer. 
*  At a given column and row.
*/
void MemoryLCD::putString(int i, int j, char *string)
{
	int index=0;
	int curI=i;
	int curJ =j;
	char asciiChar;
	FONT_CHAR_INFO curFontInfo;
	FONT_CHAR_INFO *fontInfoRomPtr;
	int stringLength;
	stringLength = strlen(string);

	for(index=0;index<stringLength;index++)
	{	
		//Pass character on to the next level. 	
		RomPutFontchar(curI,curJ,string[index]); // print this char
		
		//need character width
		fontInfoRomPtr = curFont->fontInfo;
		//convert ascii value to fontinfo array index
		asciiChar = (string[index]-32); 
		
		fontInfoRomPtr +=asciiChar;
		memcpy_P(&curFontInfo, fontInfoRomPtr, sizeof(FONT_CHAR_INFO));
		curJ +=curFontInfo.fWidth; //Bump by the width
		curJ +=1; // little space between chars
	}
}


/* 
*  Place a bitmap into the buffer. Bitmaps are simple arrays of bytes with no headers. 
*  i,j  = position of the bitmap.  
*  imgBuff = pointer to the bitmap in program memory. 
*  size_i, size_j = size of the bitmap in pixels. 
*/
void MemoryLCD::printBitmap(int i, int j, prog_uchar* imgBuff, int size_i, int size_j)
{
	unsigned char thisBmpByte;
	unsigned char lastBmpByte;
	unsigned char thisMaskByte;
	unsigned char offset;

	unsigned int iIndex = 0;
	unsigned char jByte = j/8;
	unsigned char jByteCount;
	unsigned char jByteIndex;

	for(iIndex = 0; iIndex < size_i; iIndex++)
	{
		offset = j%8;
		jByteCount = size_j / 8; //integer divide with round up
		if( size_j % 8 != 0)
		{
			jByteCount++;
		}
		if(offset == 0)
		{
			for(jByteIndex=0;jByteIndex < jByteCount;jByteIndex++)
			{
				thisBmpByte = pgm_read_byte_near(imgBuff+(iIndex*jByteCount)+jByteIndex);
				thisBmpByte = ~thisBmpByte;
				if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
				{
					if((jByte+jByteIndex>=0) && (jByte+jByteIndex < LCD_COL_BYTES))
					{
						lcdShadow[i+iIndex][jByte+jByteIndex] = revByte[thisBmpByte];
					}
				}
			}
		}
		else
		{
			//first byte is special.  do it outside the loop
			thisBmpByte = pgm_read_byte_near(imgBuff+iIndex*jByteCount);
			thisBmpByte = thisBmpByte >> offset;  //move the first byte over
			thisMaskByte = (unsigned char)0xFF >> offset;   		//build a mask;
			if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
			{
				if((jByte>=0) && (jByte < LCD_COL_BYTES))
				{			
					lcdShadow[i+iIndex][jByte] |= revByte[thisMaskByte];
					thisBmpByte = ~thisBmpByte;
					//shifted in 0's become 1's (the background color.
					lcdShadow[i+iIndex][jByte] &= revByte[thisBmpByte];  //set the bits in this byte
				}
			}

			for(jByteIndex=1;jByteIndex < jByteCount;jByteIndex++)
			{				
				//first deal with pixels left over from the last character
				//set up the bits in the first bmp byte
				lastBmpByte = pgm_read_byte_near(imgBuff+(iIndex*jByteCount)+(jByteIndex-1));
				lastBmpByte = lastBmpByte << (8 - offset);  
								
				//set up the bits in the second bmp byte (shift and mask)
				thisBmpByte = pgm_read_byte_near(imgBuff+(iIndex*jByteCount)+jByteIndex);
				thisBmpByte = thisBmpByte >> offset; 
				
				//now put it all together
				thisBmpByte = thisBmpByte | lastBmpByte;
				//Invert the colors (for correct display on sharp memory lcd)
				thisBmpByte = ~thisBmpByte; 
				//write it to the shadow (destroy any extra bits to the right)
				if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
				{
					if((jByte+jByteIndex>=0) && (jByte+jByteIndex < LCD_COL_BYTES))
					{				
						lcdShadow[i+iIndex][jByte+jByteIndex] = revByte[thisBmpByte];
					}
				}
				//first deal with pixels left over from the last character
			}
			//Last Byte is special - do this outside the loop
			//set up the bits in the second bmp byte
			thisBmpByte = pgm_read_byte_near(imgBuff+(iIndex*jByteCount)+jByteCount-1);
			thisBmpByte = ~thisBmpByte;
			thisBmpByte = thisBmpByte << (8 - offset);  
			thisBmpByte |= thisMaskByte;
			if((i+iIndex>=0) && (i+iIndex<LCD_ROWS))
			{
				if((jByte+jByteCount>=0) && (jByte+jByteCount < LCD_COL_BYTES))
				{			
					lcdShadow[i+iIndex][jByte+jByteCount] = revByte[thisBmpByte]; //use the mask
				}
			}
		}
	}
}