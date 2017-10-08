/*----------------------------------------------------------------------------
 * Name:    GLCD.c
 * Purpose: MCBSTM32C low level Graphic LCD (320x240 pixels) functions
 * Version: V1.00
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#pragma diag_suppress=550

#include "stm32f10x.h"
#include "GLCD.h"
#include "font.h"

/* SPI_SR - bit definitions. */
#define RXNE    0x01
#define TXE     0x02
#define BSY     0x80


/*********************** Hardware specific configuration **********************/

/*------------------------- Speed dependant settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N    18

/*---------------------- Graphic LCD size definitions ------------------------*/

#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#define BPP         16                  /* Bits per pixel                     */
#define BYPP        ((BPP+7)/8)         /* Bytes per pixel                    */

/*--------------- Graphic LCD interface hardware definitions -----------------*/

#define SPI_START   (0x70)              /* Start byte for SPI transfer        */
#define SPI_RD      (0x01)              /* WR bit 1 within start              */
#define SPI_WR      (0x00)              /* WR bit 0 within start              */
#define SPI_DATA    (0x02)              /* RS bit 1 within start byte         */
#define SPI_INDEX   (0x00)              /* RS bit 0 within start byte         */
 
/*---------------------------- Global variables ------------------------------*/

/******************************************************************************/
static volatile unsigned short TextColor = Black, BackColor = White;
unsigned short DeviceCode;

/************************ Local auxiliary functions ***************************/

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/

static void delay (int cnt) {

  cnt <<= DELAY_2N;
  while (cnt--);
}

/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

__inline void wr_dat (unsigned short c) {

	Set_Rs;
	Set_nRd;
  	GPIOE->ODR = c;
	Clr_nWr;
	Set_nWr;
}

/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

__inline void wr_cmd (unsigned char c) {

	Clr_Rs;
	Set_nRd;
	GPIOE->ODR = c;
	Clr_nWr;
	Set_nWr;
}

/*******************************************************************************
* Read data from LCD controller                                                *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/

__inline unsigned short rd_dat (void) {
  unsigned short val = 0;

	Set_Rs;
	Set_nWr;
	Clr_nRd;
    GPIOE->CRH = 0x44444444;
	GPIOE->CRL = 0x44444444;
	val = GPIOE->IDR;
	val = GPIOE->IDR;
	GPIOE->CRH = 0x33333333;
	GPIOE->CRL = 0x33333333;
	Set_nRd;
	return val;
}

/*******************************************************************************
* Write to LCD register                                                        *
*   Parameter:    reg:    register to be read                                  *
*                 val:    value to write to register                           *
*******************************************************************************/

__inline void wr_reg (unsigned char reg, unsigned short val) {

  	Clr_Cs;
	wr_cmd(reg);      
	wr_dat(val);    
	Set_Cs; 
}


/*******************************************************************************
* Read from LCD register                                                       *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from register                             *
*******************************************************************************/

unsigned short rd_reg (unsigned short reg) {

  	Clr_Cs;
	wr_cmd(reg);     
	reg = rd_dat();      	
	Set_Cs;
	return reg;
}


/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_init (void) { 


  /* Enable clock for GPIOA,B,C,D,E AFIO and SPI3. */
  RCC->APB2ENR |= 0x0000007D;

  /* NCS is PB2, GPIO output set to high. */
  GPIOE->CRL = 0x33333333;	    //设置PE口为50M通用推挽输出模式
  GPIOE->CRH = 0x33333333;

  GPIOD->CRH &= 0x0000ffff;	    //PD12,PD13,14,15设置为50M通用推挽输出模式
  GPIOD->CRH |= 0x33330000;

  GPIOD->BSRR = 0x0000F000;		//cs,rs,wr,rd = 1;
  wr_reg(0x00, 0x0001); 
  delay(2);                             /* Delay 50 ms                        */
  
 // DeviceCode = rd_reg(0x00);
  DeviceCode = 0x9325;
  if(DeviceCode == 0x9320)
  {
	  /* Start Initial Sequence --------------------------------------------------*/
	  wr_reg(0xE5, 0x8000);                 /* Set the internal vcore voltage     */
	  wr_reg(0x00, 0x0001);                 /* Start internal OSC                 */
	  wr_reg(0x01, 0x0100);                 /* Set SS and SM bit                  */
	  wr_reg(0x02, 0x0700);                 /* Set 1 line inversion               */
	  wr_reg(0x03, 0x1030);                 /* Set GRAM write direction and BGR=1 */
	  wr_reg(0x04, 0x0000);                 /* Resize register                    */
	  wr_reg(0x08, 0x0202);                 /* 2 lines each, back and front porch */
	  wr_reg(0x09, 0x0000);                 /* Set non-disp area refresh cyc ISC  */
	  wr_reg(0x0A, 0x0000);                 /* FMARK function                     */
	  wr_reg(0x0C, 0x0000);                 /* RGB interface setting              */
	  wr_reg(0x0D, 0x0000);                 /* Frame marker Position              */
	  wr_reg(0x0F, 0x0000);                 /* RGB interface polarity             */
	
	  /* Power On sequence -------------------------------------------------------*/
	  wr_reg(0x10, 0x0000);                 /* Reset Power Control 1              */
	  wr_reg(0x11, 0x0000);                 /* Reset Power Control 2              */
	  wr_reg(0x12, 0x0000);                 /* Reset Power Control 3              */
	  wr_reg(0x13, 0x0000);                 /* Reset Power Control 4              */
	  delay(5);                            /* Discharge cap power voltage (200ms)*/
	  wr_reg(0x10, 0x17B0);                 /* SAP, BT[3:0], AP, DSTB, SLP, STB   */
	  wr_reg(0x11, 0x0137);                 /* DC1[2:0], DC0[2:0], VC[2:0]        */
	  delay(2);                             /* Delay 50 ms                        */
	  wr_reg(0x12, 0x0139);                 /* VREG1OUT voltage                   */
	  delay(2);                             /* Delay 50 ms                        */
	  wr_reg(0x13, 0x1D00);                 /* VDV[4:0] for VCOM amplitude        */
	  wr_reg(0x29, 0x0013);                 /* VCM[4:0] for VCOMH                 */
	  delay(2);                             /* Delay 50 ms                        */
	  wr_reg(0x20, 0x0000);                 /* GRAM horizontal Address            */
	  wr_reg(0x21, 0x0000);                 /* GRAM Vertical Address              */
	
	  /* Adjust the Gamma Curve --------------------------------------------------*/
	  wr_reg(0x30, 0x0006);
	  wr_reg(0x31, 0x0101);
	  wr_reg(0x32, 0x0003);
	  wr_reg(0x35, 0x0106);
	  wr_reg(0x36, 0x0B02);
	  wr_reg(0x37, 0x0302);
	  wr_reg(0x38, 0x0707);
	  wr_reg(0x39, 0x0007);
	  wr_reg(0x3C, 0x0600);
	  wr_reg(0x3D, 0x020B);
	  
	  /* Set GRAM area -----------------------------------------------------------*/
	  wr_reg(0x50, 0x0000);                 /* Horizontal GRAM Start Address      */
	  wr_reg(0x51, (HEIGHT-1));             /* Horizontal GRAM End   Address      */
	  wr_reg(0x52, 0x0000);                 /* Vertical   GRAM Start Address      */
	  wr_reg(0x53, (WIDTH-1));              /* Vertical   GRAM End   Address      */
	  wr_reg(0x60, 0x2700);                 /* Gate Scan Line                     */
	  wr_reg(0x61, 0x0001);                 /* NDL,VLE, REV                       */
	  wr_reg(0x6A, 0x0000);                 /* Set scrolling line                 */
	
	  /* Partial Display Control -------------------------------------------------*/
	  wr_reg(0x80, 0x0000);
	  wr_reg(0x81, 0x0000);
	  wr_reg(0x82, 0x0000);
	  wr_reg(0x83, 0x0000);
	  wr_reg(0x84, 0x0000);
	  wr_reg(0x85, 0x0000);
	
	  /* Panel Control -----------------------------------------------------------*/
	  wr_reg(0x90, 0x0010);
	  wr_reg(0x92, 0x0000);
	  wr_reg(0x93, 0x0003);
	  wr_reg(0x95, 0x0110);
	  wr_reg(0x97, 0x0000);
	  wr_reg(0x98, 0x0000);
	
	  /* Set GRAM write direction and BGR = 1
	     I/D=10 (Horizontal : increment, Vertical : increment)
	     AM=1 (address is updated in vertical writing direction)                  */
	  wr_reg(0x03, 0x1038);
//	  wr_reg(0x03, 0x1018);
	
	  wr_reg(0x07, 0x0173);                 /* 262K color and display ON          */
  	}
	else if(DeviceCode==0x9325||DeviceCode==0x9328)
	{
		  /* Start Initial Sequence --------------------------------------------------*/
 	  wr_reg(0x00e5,0x78F0);                    /* Set the internal vcore voltage     */
//	  wr_reg(0x00, 0x0001);                 /* Start internal OSC                 */
	  wr_reg(0x01, 0x0100);                 /* Set SS and SM bit                  */
	  wr_reg(0x02, 0x0700);                 /* Set 1 line inversion               */
	  wr_reg(0x03, 0x1030);                 /* Set GRAM write direction and BGR=1 */
	  wr_reg(0x04, 0x0000);                 /* Resize register                    */
	  wr_reg(0x08, 0x0202);                 /* 2 lines each, back and front porch */
	  wr_reg(0x09, 0x0000);                 /* Set non-disp area refresh cyc ISC  */
	  wr_reg(0x0A, 0x0000);                 /* FMARK function                     */
	  wr_reg(0x0C, 0x0000);                 /* RGB interface setting              */
	  wr_reg(0x0D, 0x0000);                 /* Frame marker Position              */
	  wr_reg(0x0F, 0x0000);                 /* RGB interface polarity             */

	  /* Power On sequence -------------------------------------------------------*/
	  wr_reg(0x10, 0x0000);                 /* Reset Power Control 1              */
	  wr_reg(0x11, 0x0000);                 /* Reset Power Control 2              */
	  wr_reg(0x12, 0x0000);                 /* Reset Power Control 3              */
	  wr_reg(0x13, 0x0000);                 /* Reset Power Control 4              */
//	  wr_reg(0x07, 0x0001);
	  delay(5);                            /* Discharge cap power voltage (200ms)*/
	  wr_reg(0x10, 0x17B0);                 /* SAP, BT[3:0], AP, DSTB, SLP, STB   */
	  wr_reg(0x11, 0x0137);                 /* DC1[2:0], DC0[2:0], VC[2:0]        */
	  delay(2);                             /* Delay 50 ms                        */
	  wr_reg(0x12, 0x0139);                 /* VREG1OUT voltage                   */
	  delay(2);                             /* Delay 50 ms                        */
	  wr_reg(0x13, 0x1D00);                 /* VDV[4:0] for VCOM amplitude        */
	  wr_reg(0x29, 0x0013);                 /* VCM[4:0] for VCOMH                 */
	  
	  delay(2);                             /* Delay 50 ms                        */
	  wr_reg(0x20, 0x0000);                 /* GRAM horizontal Address            */
	  wr_reg(0x21, 0x0000);                 /* GRAM Vertical Address              */

		delay(5);

		  /* Adjust the Gamma Curve --------------------------------------------------*/
	  wr_reg(0x30, 0x0006);
	  wr_reg(0x31, 0x0101);
	  wr_reg(0x32, 0x0003);
	  wr_reg(0x35, 0x0106);
	  wr_reg(0x36, 0x0B02);
	  wr_reg(0x37, 0x0302);
	  wr_reg(0x38, 0x0707);
	  wr_reg(0x39, 0x0007);
	  wr_reg(0x3C, 0x0600);
	  wr_reg(0x3D, 0x020B);

        delay(5);
			  /* Set GRAM area -----------------------------------------------------------*/
	  wr_reg(0x50, 0x0000);                 /* Horizontal GRAM Start Address      */
//	  wr_reg(0x51, (HEIGHT-1));             /* Horizontal GRAM End   Address      */
	  wr_reg(0x51, 0x00ef);
	  wr_reg(0x52, 0x0000);                 /* Vertical   GRAM Start Address      */
//	  wr_reg(0x53, (WIDTH-1));              /* Vertical   GRAM End   Address      */
	  wr_reg(0x53, 0x013f);
	  wr_reg(0x60, 0xa700);                 /* Gate Scan Line 扼牢 菊第函券       */
	  wr_reg(0x61, 0x0001);                 /* NDL,VLE, REV                       */
	  wr_reg(0x6A, 0x0000);                 /* Set scrolling line                 */

        wr_reg(0x0080,0x0000);
        wr_reg(0x0081,0x0000);
        wr_reg(0x0082,0x0000);
        wr_reg(0x0083,0x0000);
        wr_reg(0x0084,0x0000);
        wr_reg(0x0085,0x0000);
      
        wr_reg(0x0090,0x0010);     
        wr_reg(0x0092,0x0600);  
        wr_reg(0x0007,0x0133);
	    
        wr_reg(0x0020,0x0000);                                                            
        wr_reg(0x0021,0x0000);

  /* Panel Control -----------------------------------------------------------*/
	  wr_reg(0x90, 0x0010);
	  wr_reg(0x92, 0x0000);
	  wr_reg(0x93, 0x0003);
	  wr_reg(0x95, 0x0110);
	  wr_reg(0x97, 0x0000);
	  wr_reg(0x98, 0x0000);
	
	  /* Set GRAM write direction and BGR = 1
	     I/D=10 (Horizontal : increment, Vertical : increment)
	     AM=1 (address is updated in vertical writing direction)                  */
	  wr_reg(0x03, 0x1038);
//	  wr_reg(0x03, 0x1018);
	
	  wr_reg(0x07, 0x0173);                 /* 262K color and display ON          */
	}
	else if(DeviceCode==0x8999)	   //对应的驱动IC为SSD1289
	{
		//************* Start Initial Sequence **********//
		wr_reg(0x00, 0x0001); // Start internal OSC.
		wr_reg(0x01, 0x3B3F); // Driver output control, RL=0;REV=1;GD=1;BGR=0;SM=0;TB=1
		wr_reg(0x02, 0x0600); // set 1 line inversion
		//************* Power control setup ************/
		wr_reg(0x0C, 0x0007); // Adjust VCIX2 output voltage
		wr_reg(0x0D, 0x0006); // Set amplitude magnification of VLCD63
		wr_reg(0x0E, 0x3200); // Set alternating amplitude of VCOM
		wr_reg(0x1E, 0x00BB); // Set VcomH voltage
		wr_reg(0x03, 0x6A64); // Step-up factor/cycle setting
		//************ RAM position control **********/
		wr_reg(0x0F, 0x0000); // Gate scan position start at G0.
		wr_reg(0x44, 0xEF00); // Horizontal RAM address position
		wr_reg(0x45, 0x0000); // Vertical RAM address start position
		wr_reg(0x46, 0x013F); // Vertical RAM address end position
		// ----------- Adjust the Gamma Curve ----------//
		wr_reg(0x30, 0x0000);
		wr_reg(0x31, 0x0706);
		wr_reg(0x32, 0x0206);
		wr_reg(0x33, 0x0300);
		wr_reg(0x34, 0x0002);
		wr_reg(0x35, 0x0000);
		wr_reg(0x36, 0x0707);
		wr_reg(0x37, 0x0200);
		wr_reg(0x3A, 0x0908);
		wr_reg(0x3B, 0x0F0D);
		//************* Special command **************/
		wr_reg(0x28, 0x0006); // Enable test command
		wr_reg(0x2F, 0x12EB); // RAM speed tuning
		wr_reg(0x26, 0x7000); // Internal Bandgap strength
		wr_reg(0x20, 0xB0E3); // Internal Vcom strength
		wr_reg(0x27, 0x0044); // Internal Vcomh/VcomL timing
		wr_reg(0x2E, 0x7E45); // VCOM charge sharing time  
		//************* Turn On display ******************/
		wr_reg(0x10, 0x0000); // Sleep mode off.
		delay(8); // Wait 30mS
		wr_reg(0x11, 0x6878); // Entry mode setup. 262K type B, take care on the data bus with 16it only
		wr_reg(0x07, 0x0033); // Display ON	*/
	}
}

void GLCD_putPixel(unsigned int x, unsigned int y) {
  wr_reg(0x20, x);
  wr_reg(0x21, y);
  wr_reg(0x22,TextColor);
}


/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_setTextColor(unsigned short color) {
  TextColor = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_setBackColor(unsigned short color) {
  BackColor = color;
}
/*******************************************************************************
*LCD_SetCursor                                                         *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/
__inline void LCD_SetCursor(u8 Xpos, u16 Ypos)
{
 	if(DeviceCode==0x8999||DeviceCode==0x9919)
	{
		wr_reg(0x004E, Xpos);
		wr_reg(0X004F, Ypos);
	}
	else 
	{
		wr_reg(0x0020, Xpos);
		wr_reg(0X0021, Ypos);
	}
}
/*******************************************************************************
*LCD_WindowMax                                                         *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/
void LCD_WindowMax (unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end) {

	if(DeviceCode==0x8999)
	{
		wr_reg(0x44,x|((x_end-1)<<8));
		wr_reg(0x45,y);
		wr_reg(0x46,y_end-1);
	}
	else
	{
		wr_reg(0x50, x);                      /* Horizontal GRAM Start Address      */
		wr_reg(0x51, x_end-1);               /* Horizontal GRAM End   Address (-1) */
		wr_reg(0x52, y);                      /* Vertical   GRAM Start Address      */
		wr_reg(0x53, y_end-1);                /* Vertical   GRAM End   Address (-1) */
	}
}
/*******************************************************************************
* Clear display                                                                *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_clear (unsigned short color) {
  unsigned int   i;
  LCD_WindowMax(0,0,240,320);
  LCD_SetCursor(0,0);
  Clr_Cs;
  wr_cmd(0x22);
  for(i = 0; i < (WIDTH*HEIGHT); i++)
  {
  	wr_dat(color);
  }
  Set_Cs;  
}


/*******************************************************************************
* Draw character on given position (line, coloum                               *
*   Parameter:     x :        horizontal position                              *
*                  y :        vertical position                                *
*                  c*:        pointer to color value                           *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_drawChar(unsigned int x, unsigned int y, unsigned short *c) {
  unsigned int index = 0;
  int  i = 0;
  unsigned int Xaddress = 0;
   
  Xaddress = x;
  LCD_SetCursor(Xaddress,y);


  for(index = 0; index < 24; index++)
  {
  	Clr_Cs;
    wr_cmd(0x22);              /* Prepare to write GRAM */
    for(i = 15; i >= 0; i--)
//    for(i = 0; i < 16; i++)
    {
      if((c[index] & (1 << i)) == 0x00) {
        wr_dat(BackColor);
      } else {
        wr_dat(TextColor);
      }
    }
	Set_Cs;
    Xaddress++;
  LCD_SetCursor(Xaddress,y);
  }
}


/*******************************************************************************
* Disply character on given line                                               *
*   Parameter:     c :        ascii character                                  *
*                  ln:        line number                                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_displayChar(unsigned int ln, unsigned int col, unsigned char  c) {
  c -= 32;
  GLCD_drawChar(ln, col, &ASCII_Table[c * 24]);
}


/*******************************************************************************
* Disply string on given line                                                  *
*   Parameter:     s*:        pointer to string                                *
*                  ln:        line number                                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_displayStringLn(unsigned int ln, unsigned char *s) {
  unsigned int i = 0;
  unsigned int refcolumn = (WIDTH/*-1*/)-16;

  while ((*s != 0) & (i < 20))                   /* write the string character by character on lCD */
  {
    GLCD_displayChar(ln, refcolumn, *s);         /* Display one character on LCD */
    refcolumn -= 16;                             /* Decrement the column position by 16 */
    s++;                                         /* Point on the next character */
    i++;                                         /* Increment the character counter */
  }
}


/*******************************************************************************
* Clear given line                                                             *
*   Parameter:     ln:        line number                                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_clearLn(unsigned int ln) {
  GLCD_displayStringLn(ln, "                    ");
}


/*******************************************************************************
* Display graphical bitmap image at position x horizontally and y vertically   *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bitmap:   address at which the bitmap data resides         *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) {
  unsigned int   i;
  unsigned int   len = w*h;
  unsigned short *bitmap_ptr = (unsigned short *)bitmap;

  wr_reg(0x50, y);                      /* Horizontal GRAM Start Address      */
  wr_reg(0x51, y+h-1);                  /* Horizontal GRAM End   Address (-1) */
  wr_reg(0x52, x);                      /* Vertical   GRAM Start Address      */
  wr_reg(0x53, x+w-1);                  /* Vertical   GRAM End   Address (-1) */

  wr_reg(0x20, y);
  wr_reg(0x21, x);

  Clr_Cs;
  wr_cmd(0x22);
  for (i = 0; i < len; i++) {
    wr_dat(*bitmap_ptr++);
  }
  Set_Cs;
}


void GLCD_Test(void)
{
  u16 i,j;
  LCD_SetCursor(0,0);

  Clr_Cs; 
  wr_cmd(0x22);
  
  for(i=0;i<320;i++)
    for(j=0;j<240;j++)
    {
      if(i>279)wr_dat(0x0000);
      else if(i>239)wr_dat(0x001f);
      else if(i>199)wr_dat(0x07e0);
      else if(i>159)wr_dat(0x07ff);
      else if(i>119)wr_dat(0xf800);
      else if(i>79)wr_dat(0xf81f);
      else if(i>39)wr_dat(0xffe0);
      else wr_dat(0xffff);
    }
  Set_Cs;
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
  wr_cmd(0x22);
}
/*******************************************************************************
* Display graphical bitmap image at position x horizontally and y vertically   *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bitmap:   address at which the bitmap data resides         *
*   Return:                                                                    *
*******************************************************************************/
void LCD_WriteBMP(u8 Xpos, u16 Ypos, u8 Height, u16 Width, u8 *bitmap)
{
  vu32 index;
  vu32 size = Height * Width;
  u16 *bitmap_ptr = (u16 *)bitmap;

  LCD_WindowMax(Xpos, Ypos, Xpos+Height, Ypos+Width);
  LCD_SetCursor(Xpos,Ypos);
  wr_reg(0x03, 0x1038);
  Clr_Cs;
  wr_cmd(0x22);
  for(index = 0; index < size; index++)\
  {
    wr_dat(*bitmap_ptr++);
  }
  Set_Cs;
}

