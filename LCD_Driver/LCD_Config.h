
#include "STM32F10x_lib.h"

#define Dis_X_MAX		480-1
#define Dis_Y_MAX		320-1

//Define the MCU Register
#define LCD_Ctrl_GPIO()		//PINSEL1 &= ~(0x00003cfc)
#define LCD_Ctrl_Out()		//IODIR0 |= (SPI_SS+SPI_SDA+SPI_SCK+SPI_RES)
#define LCD_Ctrl_In()		//IODIR0 &= ~(SPI_BUSY)
#define LCD_Ctrl_Set(n)		GPIOB->BSRR = n
#define LCD_Ctrl_Clr(n)		GPIOB->BSRR = ((n)<<16)

#define	LCD_RST				(unsigned int)(0x01<<10)
#define	LCD_RST_SET()	GPIOB->BSRR = LCD_RST
#define	LCD_RST_CLR()	GPIOB->BSRR = (LCD_RST<<16)

#define	LCD_PS				(unsigned int)(0x01<<11)
#define	LCD_PS_SET()	GPIOB->BSRR = LCD_PS
#define	LCD_PS_CLR()	GPIOB->BSRR = (LCD_PS<<16)

#define SPI_SS				(unsigned int)(0x01<<12)
#define SPI_SS_SET()		GPIOB->BSRR = SPI_SS
#define SPI_SS_CLR()		GPIOB->BSRR = (SPI_SS<<16)

#define SPI_SDA				(unsigned int)(0x01<<15)
#define SPI_SDA_SET()		GPIOB->BSRR = SPI_SDA
#define SPI_SDA_CLR()		GPIOB->BSRR = (SPI_SDA<<16)

#define SPI_SCK				(unsigned int)(0x01<<13)
#define SPI_SCK_SET()		GPIOB->BSRR = SPI_SCK
#define SPI_SCK_CLR()		GPIOB->BSRR = (SPI_SCK<<16)

#define LCD_ASC8_FONT						1
#define LCD_ASC12_FONT						2
#define LCD_ASC16_FONT						3

#define LCD_HZK16_FONT						4
#define LCD_HZK24_FONT						5
#define LCD_HZK32_FONT						6


#define CLEAR_SCREEN				0x80
#define SET_BG_COLOR				0x81
#define SET_FG_COLOR				0x82
#define SET_LCD_BRIGHTNESS	0x83
#define SET_BR							0x84
#define ASC_DISPLAY					0x85
#define ASC_STR_DISPLAY			0x86
#define HZ_DISPLAY					0x87
#define HZ_STR_DISPLAY			0x88
#define ICON_DISPLAY				0x89

#define DRAW_POINT					0x8a
#define DRAW_LINE						0x8b
#define DRAW_RECT						0x8C
#define DRAW_SOLID_RECT			0x8D
#define DRAW_CIRCLE					0x8E
#define DRAW_SOLID_CIRCLE		0x8F

#define	CMD									0X00FF
#define	DAT									0X0100


#define LCD_RED 0xF800
#define LCD_GREEN 0x07e0
#define LCD_BLUE  0x001F
#define LCD_LIGHT_BLUE 0x075F
#define LCD_DARK 0x0000
#define LCD_WHITE 0xFFFF	
#define LCD_MIDDLE_BLE	0x74F9
#define LCD_DEEP_BLUE	0x10AF
