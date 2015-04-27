#ifndef	__LCD_DIS_h__
#define	__LCD_DIS_h__
//	write your header here
#if 0
extern void TimeDelay(unsigned int Timers);
extern void LCD_Init(void);
extern void SPI_SSSet(unsigned char Status);
extern void SPI_Send(unsigned char Data);

extern void PutChar(unsigned int x,unsigned int y,unsigned char a,unsigned char type);
extern void PutString(unsigned int x,unsigned int y,unsigned char *p);
extern void PutChar_cn(unsigned int x,unsigned int y,unsigned char * GB,unsigned char type);
void PutString_cn(unsigned int x,unsigned int y,unsigned char *p);
extern void PutPixel(unsigned int x,unsigned int y);
extern void Line(unsigned int s_x,unsigned int  s_y,unsigned int  e_x,unsigned int  e_y);
extern void Circle(unsigned int x,unsigned int y,unsigned int r,unsigned char mode);
extern void Rectangle(unsigned int left, unsigned int top, unsigned int right,
				 unsigned int bottom, unsigned char mode);
extern void ClrScreen(void);
extern void PutBitmap(unsigned char *p,unsigned long length);
extern void ShowFont_ASC(unsigned int x,unsigned int y,unsigned char a,unsigned char type) ;
extern void ShowFont_CN(unsigned int x,unsigned int y,unsigned short a,unsigned char type) ;
extern void SetBackLight(unsigned char brightness);

extern void SetBG_Color(unsigned int BGColor);
extern void SetFG_Color(unsigned int FGColor);

extern void SetDisplayOnOff(unsigned char OnOff);
extern void SetDisplayInvert(unsigned char flag);

extern void SetColumnAdd(unsigned int ColStartAddr,unsigned int ColEndAddr);
extern void SetRowAdd(unsigned int RowStartAddr,unsigned int RowEndAddr);
extern void SetWindows(unsigned int ColStartAddr,unsigned int ColEndAddr,unsigned int RowStartAddr,unsigned int RowEndAddr);
extern void SetFontType(unsigned char FontType);
extern unsigned char GetKey(void);
extern void LedON(unsigned char on);
extern void Delay_40US(void);
#else
extern void TimeDelay(unsigned int Timers);
extern void LCD_Init(void);
extern void SPI_SSSet(unsigned char Status);
extern void SPI_Send(unsigned int Data);

extern void PutChar(unsigned int x,unsigned int y,unsigned char a,unsigned char type);
extern void PutString(unsigned int x,unsigned int y,unsigned char *p,unsigned char type);
extern void PutChar_cn(unsigned int x,unsigned int y,unsigned char * GB,unsigned char type);
void PutString_cn(unsigned int x,unsigned int y,unsigned char *p,unsigned char type);
extern void PutPixel(unsigned int x,unsigned int y);
extern void Line(unsigned int s_x,unsigned int  s_y,unsigned int  e_x,unsigned int  e_y);
extern void Circle(unsigned int x,unsigned int y,unsigned int r,unsigned char mode);
extern void Rectangle(unsigned int left, unsigned int top, unsigned int right,
				 unsigned int bottom, unsigned char mode);
extern void ClrScreen(void);
extern void PutBitmap(unsigned int x,unsigned int y,unsigned long p);
extern void ShowFont_ASC(unsigned int x,unsigned int y,unsigned char a,unsigned char type) ;
extern void ShowFont_CN(unsigned int x,unsigned int y,unsigned short a,unsigned char type) ;
extern void SetBackLight(unsigned char brightness);

extern void SetBG_Color(unsigned int BGColor);
extern void SetFG_Color(unsigned int FGColor);
extern unsigned char GetKey(void);
#endif
#endif
