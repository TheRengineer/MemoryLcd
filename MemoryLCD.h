/*****************************************************************************
 *
 *  Project Name:       Sharp Memory Lcd Library
 *  File Name:          MemoryLcd.h
 *  Author:             Adam Fabio
 *  Creation Date:      03/12/2012
 *
 *  Description:   		This library provides an interface to the 
 * 						sharp memory lcd. 
 *
 ******************************************************************************/


#ifndef MEMORYLCD_H
#define MEMORYLCD_H
#include "Arduino.h"
//#include <SPI.h>
#define SLAVE_SELECT_PIN 10
#define DISP_ENABLE 4
#define LCD_DELAY 3
#define vHighBit  0x02;//0100
#define vLowBit  0x00;
#define vBitMask 0xFD; //1011 1111

#define LCD_ROWS  96
#define LCD_COLS  96
#define LCD_COL_BYTES LCD_COLS/8
#define CLOUD 0xFF
#define SHINE 0x00
/*Commands*/
#define CMD_CLEAR 0x04
#define CMD_WRITE_ROW 0x01
#define uint_8 unsigned char

#define EXT_HARDWARE 0
#define EXT_SOFTWARE 1

#define MAX_NUM_FONTS 5
typedef struct F_C_I 
{
	unsigned char fWidth;
	unsigned char fHeight;
	//unsigned short fgColor;
	unsigned int offset;
	
}FONT_CHAR_INFO;

typedef struct FontData
{
 char fontName[10];
 FONT_CHAR_INFO *fontInfo;
 unsigned char *fontBmap;
}FONT_DATA;

class MemoryLCD 
{
	public:
		MemoryLCD(unsigned char inMode);
        ~MemoryLCD();
		void begin();
        void enable();
        void disable();
        void clear();
		void refresh();
		void sendRow(int row);
		void setPixel(int i,int j,unsigned char color,unsigned char sendNow);
		void printBitmap(int i, int j, prog_uchar* imgBuff, int size_i, int size_j);	
		char checkVBit();
		
		void plotCircle(int xm, int ym, int r,unsigned char _color);
		void PlotLine(int x0, int y0, int x1, int y1,unsigned char _color);
		void Circle(int x0, int y0, int radius, unsigned char color);
		void RomPutFontchar(int i, int j,  unsigned char asciiChar);
		void putString(int i, int j, char *string);
		void invertColors();
		unsigned char addFont(char *name, FONT_CHAR_INFO *fontInfo,unsigned char *fontBmap);
	    unsigned char checkFont(char *lkupFont);
		unsigned char setFont(char *lkupFont);
			unsigned char lcdShadow[LCD_ROWS][LCD_COL_BYTES];
			FONT_DATA fontList2[MAX_NUM_FONTS];
			unsigned char curFontCount;
	private:
		//unsigned char lcdShadow[LCD_ROWS*LCD_COLS];
		unsigned char bgColor;
		char extMode;
		char cur_vBit;
		void handleVbit(char *cmd);
		FontData *curFont;
		unsigned long  prev_ms;
		unsigned long  vbitIntervalMs;
};
#endif /*MEMORYLCD_H*/

