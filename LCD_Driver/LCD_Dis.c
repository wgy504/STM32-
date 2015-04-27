//========================================================================
// 文件名:  LCD_Dis.c
//========================================================================
#include "LCD_Config.h"
//#include "stm32f10x_lib.h"


//========================================================================
// 函数: void LCD_Init(void)
// 描述: LCD初始化程序，主要在里面完成端口初始化以及LCD模块的复位
//========================================================================

void TimeDelay(unsigned int Timers)
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<500;i++);
	}
}

void LCD_Init(void)
{
	//SCK预先设置为高电平
	SPI_SCK_SET();
	//CS预先设置为高电平
	SPI_SS_SET();
	
	TimeDelay(80);				    	//延时大概10ms左右
}
//========================================================================
// 函数: void delay2(unsigned char i)
// 描述: 延时函数
//		delay2(0):延时518us 518-2*256=6
//　　delay2(1):延时7us
//　　delay2(10):延时25us 25-20=5
//　　delay2(20):延时45us 45-40=5
//　　delay2(100):延时205us 205-200=5
//　　delay2(200):延时405us 405-400=5
//========================================================================
void delay2(unsigned char i)
{
	while(--i);
} 

unsigned char GetKey(void)
{
	/*
	if(key1==0)
	{
			TimeDelay(2);	  //消抖动
			if(key1==0)	  //确认按键被按下
			{
				while(!key1);//松手检测
				return 1;
			}
	}
*/
	return 0;
}
//========================================================================
// 函数: void SPI_SSSet(unsigned char Status)
// 描述: 置SS线状态
//========================================================================
void SPI_SSSet(unsigned char Status)
{
	if(Status)				//判断是要置SS为低还是高电平？
		SPI_SS_SET();			//SS置高电平
	else   						
		SPI_SS_CLR();			//SS置低电平
}
//========================================================================
// 函数: void SPI_Send(unsigned char Data)
// 描述: 通过串行SPI口输送一个byte的数据置模组
//========================================================================
void SPI_Send(unsigned int Data)
{
	unsigned char i=0;
	for(i=0;i<9;i++)
	{
		if(Data&0x100)
			SPI_SDA_SET();							//SDA置高
		else 
			SPI_SDA_CLR();						//SDA置低
		SPI_SCK_CLR();								//SCK置低
		delay2(30);//2us
		SPI_SCK_SET();								//SCK置高
		delay2(30);//2us
		Data = Data<<1;							//数据左移一位
	}
}

//========================================================================
// 函数: void PutChar(unsigned int x,unsigned int y,unsigned char a,unsigned char type) 
// 描述: 写入一个标准ASCII字符
//========================================================================
void PutChar(unsigned int x,unsigned int y,unsigned char a,unsigned char type) 
{
	//显示ASCII字符
	SPI_SS_CLR();														//SS置低电平	
	SPI_Send(ASC_DISPLAY&CMD);										//传送指令ASC_DISPLAY
	SPI_Send((x>>8)|DAT);					//要显示字符的左上角的X轴位置
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);					//要显示字符的左上角的Y轴位置
	SPI_Send(y|DAT);
	SPI_Send(type|DAT);
	SPI_Send(a|DAT);															//要显示的字符数据
	SPI_SS_SET();															//完成操作置SS高电平
}
//========================================================================
// 函数: void PutString(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
// 描述: 在x、y为起始坐标处写入一串标准ASCII字符串
//========================================================================
void PutString(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
{
    unsigned char width,i;
    if(type==LCD_ASC8_FONT)
        width=8;
    else if(type==LCD_ASC12_FONT)
        width=12;
    else if(type==LCD_ASC16_FONT)
        width=16;
    for(i=0;i<strlen((char *)p);i++){
        PutChar(x+i*width,y,p[i],type);
        //delay2(200);
    }
}
//========================================================================
// 函数: void PutChar_cn(unsigned int x,unsigned int y,unsigned char * GB,unsigned char type) 
// 描述: 写入一个二级汉字库汉字
//========================================================================
void PutChar_cn(unsigned int x,unsigned int y,unsigned char * GB,unsigned char type) 
{
	//显示汉字字符
	SPI_SS_CLR();															//SS置低电平	
	SPI_Send(HZ_DISPLAY&CMD);											//传送指令HZ_DISPLAY
	SPI_Send((x>>8)|DAT);					//要显示字符的左上角的X轴位置
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);					//要显示字符的左上角的Y轴位置
	SPI_Send(y|DAT);
	SPI_Send(type|DAT);
	SPI_Send((*(GB++))|DAT);												//传送二级字库中汉字GB码的高八位值
	SPI_Send((*GB)|DAT);														//传送二级字库中汉字GB码的低八位值
	SPI_SS_SET();															//完成操作置SS高电平
}
//========================================================================
// 函数: void PutString_cn(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
// 描述: 在x、y为起始坐标处写入一串汉字字符串
//========================================================================
void PutString_cn(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
{
    unsigned char width,i;
    if(type==LCD_HZK16_FONT)
        width=16;
    else if(type==LCD_HZK24_FONT)
        width=24;
    else if(type==LCD_HZK32_FONT)
        width=32;
    for(i=0;i<strlen((char *)p)-1;i+=2){
        PutChar_cn(x+(i/2)*width,y,&p[i],type);
        //delay2(200);
    }
}

//========================================================================
// 函数: void PutPixel(unsigned int x,unsigned int y)
// 描述: 在x、y点上绘制一个前景色的点
//========================================================================
void PutPixel(unsigned int x,unsigned int y)
{
	//绘点操作	
	SPI_SS_CLR();															//SS置低电平
	SPI_Send(DRAW_POINT&CMD);											//送指令DRAW_POINT
	SPI_Send((x>>8)|DAT);					//送第一个数据,即设置点的X轴位置
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);					//点的Y轴位置
	SPI_Send(y|DAT);
	SPI_SS_SET();															//完成操作置SS高电平
}
//========================================================================
// 函数: void Line(unsigned int s_x,unsigned int  s_y,unsigned int  e_x,unsigned int  e_y)
// 描述: 在s_x、s_y为起始坐标，e_x、e_y为结束坐标绘制一条直线
//========================================================================
void Line(unsigned int s_x,unsigned int  s_y,unsigned int  e_x,unsigned int  e_y)
{  
	//绘制直线
	SPI_SS_CLR();															//SS置低电平
	SPI_Send(DRAW_LINE&CMD);												//送指令DRAW_LINE
	SPI_Send((s_x>>8)|DAT);					//起点X轴坐标
	SPI_Send(s_x|DAT);
	SPI_Send((s_y>>8)|DAT);					//起点Y轴坐标
	SPI_Send(s_y|DAT);
	SPI_Send((e_x>>8)|DAT);					//终点X轴坐标
	SPI_Send(e_x|DAT);
	SPI_Send((e_y>>8)|DAT);					//终点Y轴坐标
	SPI_Send(e_y|DAT);
	SPI_SS_SET();																//完成操作置SS高电平
}
//========================================================================
// 函数: void Circle(unsigned int x,unsigned int y,unsigned int r,unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
//========================================================================
void Circle(unsigned int x,unsigned int y,unsigned int r,unsigned char mode)
{
	SPI_SS_CLR();													//SS置低电平
	if(mode)
		SPI_Send(DRAW_SOLID_CIRCLE&CMD);				//送指令DRAW_SOLID_CIRCLE
	else
		SPI_Send(DRAW_CIRCLE&CMD);							//送指令DRAW_CIRCLE
	SPI_Send((x>>8)|DAT);			//起点X轴坐标
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);			//终点Y轴坐标
	SPI_Send(y|DAT);
	SPI_Send((r>>8)|DAT);			//圆半径r
	SPI_Send(r|DAT);
	SPI_SS_SET();													//完成操作置SS高电平
}
//========================================================================
// 函数: void Rectangle(unsigned int left, unsigned int top, unsigned int right,
//				 unsigned int bottom, unsigned char mode)
// 描述: 以起点和终点画方形，方形(mode = 0) or 实心方形(mode = 1)
//========================================================================
void Rectangle(unsigned int left, unsigned int top, unsigned int right,
				 unsigned int bottom, unsigned char mode)
{
	SPI_SS_CLR();														//SS置低电平
	if(mode)
		SPI_Send(DRAW_SOLID_RECT&CMD);						//送指令DRAW_SOLID_RECT
	else
		SPI_Send(DRAW_RECT&CMD);									//送指令DRAW_RECT
	SPI_Send((left>>8)|DAT);
	SPI_Send(left|DAT);
	SPI_Send((top>>8)|DAT);
	SPI_Send(top|DAT);
	SPI_Send((right>>8)|DAT);
	SPI_Send(right|DAT);
	SPI_Send((bottom>>8)|DAT);
	SPI_Send(bottom|DAT);
	SPI_SS_SET();														//完成操作置SS高电平
}
//========================================================================
// 函数: void ClrScreen(void)
// 描述: 清屏函数，执行全屏幕清除
//========================================================================
void ClrScreen(void)
{
	//清屏操作
	SPI_SS_CLR();										//SS置低电平
	SPI_Send(CLEAR_SCREEN&CMD);					//送指令CLEAR_SCREEN
	SPI_SS_SET();										//完成操作置SS高电平
}

//========================================================================
// 函数: void SetBG_Color(unsigned int BGColor)
// 描述: 设置背景色
//========================================================================
void SetBG_Color(unsigned int BGColor)
{
	
	SPI_SS_CLR();														//SS置低电平
	SPI_Send(SET_BG_COLOR&CMD);									//送指令SET_BG_COLOR
	SPI_Send((BGColor>>8)|DAT);  //背景色，16bit RGB565
	SPI_Send(BGColor|DAT);
	SPI_SS_SET();														//完成操作置SS高电平
}

//========================================================================
// 函数: SetFG_Color(unsigned int FGColor)
// 描述: 设置前景色
//========================================================================
void SetFG_Color(unsigned int FGColor)
{
	SPI_SS_CLR();														//SS置低电平
	SPI_Send(SET_FG_COLOR&CMD);									//送指令SET_FG_COLOR
	SPI_Send((FGColor>>8)|DAT);	//前景色，16bit RGB565
	SPI_Send(FGColor|DAT);
	SPI_SS_SET();														//完成操作置SS高电平
}
//========================================================================
// 函数: void PutBitmap(unsigned int x,unsigned int y,unsigned long p)
// 描述: 在x、y为起始坐标显示一个单色位图
//========================================================================
void PutBitmap(unsigned int x,unsigned int y,unsigned long p)
{

	SPI_SS_CLR();												//SS置低电平
	SPI_Send(ICON_DISPLAY&CMD);							//送指令ICON_DISPLAY
	SPI_Send((x>>8)|DAT);		//起点X轴坐标
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);		//起点Y轴坐标
	SPI_Send(y|DAT);
	SPI_Send(((unsigned int)(p>>24))|DAT);		//bmp 存放在flash地址，DWORD 类型
	SPI_Send(((unsigned int)(p>>16))|DAT);
	SPI_Send(((unsigned int)(p>>8))|DAT);
	SPI_Send(((unsigned int)p)|DAT);
	SPI_SS_SET();												//完成操作置SS高电平
}


//========================================================================
// 函数: void SetBackLight(unsigned char Deg)
// 描述: 设置背光亮度等级
//========================================================================
void SetBackLight(unsigned char brightness) 
{
	//显示ASCII字符
	SPI_SS_CLR();													//SS置低电平	
	SPI_Send(SET_LCD_BRIGHTNESS&CMD);					//传送指令SET_LCD_BRIGHTNESS
	SPI_Send(brightness|DAT);												//背光亮度 0：关闭 1~9 亮度调整
	SPI_SS_SET();													//完成操作置SS高电平
}


