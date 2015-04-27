#include "sim900a.h"
#include "../delay/delay.h"
#include "globals.h"

extern u16 USART3_RX_STA;							//接收数据状态


//SIM900A驱动

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
//unicode gbk 转换函数
//src:输入字符串
//dst:输出(uni2gbk时为gbk内码,gbk2uni时,为unicode字符串)
//mode:0,unicode到gbk转换;
//     1,gbk到unicode转换; 
/*void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
	u16 temp; 
	u8 buf[2];
	if(mode)//gbk 2 unicode
	{
		while(*src!=0)
		{
			if(*src<0X81)	//非汉字
			{
				temp=(u16)ff_convert((WCHAR)*src,1);
				src++;
			}else 			//汉字,占2个字节
			{
				buf[1]=*src++;
				buf[0]=*src++; 
				temp=(u16)ff_convert((WCHAR)*(u16*)buf,1); 
			}
			*dst++=sim900a_hex2chr((temp>>12)&0X0F);
			*dst++=sim900a_hex2chr((temp>>8)&0X0F);
			*dst++=sim900a_hex2chr((temp>>4)&0X0F);
			*dst++=sim900a_hex2chr(temp&0X0F);
		}
	}else	//unicode 2 gbk
	{ 
		while(*src!=0)
		{
			buf[1]=sim900a_chr2hex(*src++)*16;
			buf[1]+=sim900a_chr2hex(*src++);
			buf[0]=sim900a_chr2hex(*src++)*16;
			buf[0]+=sim900a_chr2hex(*src++);
 			temp=(u16)ff_convert((WCHAR)*(u16*)buf,0);
			if(temp<0X80){*dst=temp;dst++;}
			else {*(u16*)dst=swap16(temp);dst+=2;}
		} 
	}
	*dst=0;//添加结束符
} 
*/
//收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//		1,清零USART2_RX_STA；
void sim_at_response(u8 mode)
{
	if(USART3_RX_STA & 0x8000)		//接收到一次数据
	{
		USART2_RX_BUF[USART3_RX_STA & 0x7FFF] = 0; //添加结束符
		printf("%s", USART2_RX_BUF);  //发送到串口
		if(mode) USART3_RX_STA = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM900A 各项测试(拨号测试、短信测试、GPRS测试)共用代码

//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx = 0;
	if(USART3_RX_STA & 0x8000)		//接收到一次数据
	{
		USART2_RX_BUF[USART3_RX_STA & 0x7FFF] = 0;  //添加结束符
		printf("%s", USART2_RX_BUF);  //发送到串口
		strx = strstr((const char*)USART2_RX_BUF,(const char*)str);
	}
	return (u8*)strx;
}

//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res = 0;
	USART3_RX_STA = 0;
	if((u32)cmd <= 0xFF)
	{
		while(DMA1_Channel2->CNDTR != 0);	//等待通道2传输完成
		USART3->DR = (u32)cmd;
	}
	else
		u2_printf("%s\r\n",cmd);	//发送命令
	if(ack && waittime)		//需要等待应答、
	{
		while(--waittime)		//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA & 0x8000) //接收到期待的应答结果
			{
				if(sim900a_check_cmd(ack)) break;	//得到有效数据
				USART3_RX_STA = 0;
			}
		}
		if(waittime == 0) res = 1;
	}
	return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//拨号测试部分代码

//sim900a拨号测试
//用于拨打电话和接听电话
//返回值:0,正常
//    其他,错误代码
u8 sim900a_call_test(void)
{
	u8 phonenumber[20];
	u8 *temp;
	u16 len,i;
	u8 key;
	u8 *p,*p1,*p2;
	u8 oldmode = 0;
	u8 cmode = 0; //模式
				//0：等待拨号；1：拨号中
				//2：通话中；3：接收到来电
	if(sim900a_send_cmd("AT+CLIP=1","OK",200)) return 1; //设置来电显示 
	if(sim900a_send_cmd("AT+COLP=1","OK",200)) return 2;  //设置被叫号码显示
	printf("ATATK-SIM900A\r\n");

	while(1)
	{
		printf("按KEYLEFT拨打/接通电话\r\n");
		printf("按KEYRIGHT挂断电话\r\n");
		delay_ms(10);
		if(USART3_RX_STA & 0x8000)		//接收到数据
		{
			sim_at_response(0);
			if(cmode==1 || cmode==2)
			{
				if(cmode==1)
					if(sim900a_check_cmd("+COLP:")) cmode = 2; //拨号成功
				if(sim900a_check_cmd("NO CARRIER")) cmode = 0; //拨号失败
				if(sim900a_check_cmd("NO ANSWER")) cmode = 0;  //拨号失败
				if(sim900a_check_cmd("ERROR")) cmode = 0;	   //拨号失败
			} 
			if(sim900a_check_cmd("+CLIP:"))		//接收到来电
			{
				LED0 = 0;
				cmode = 3;
				p = sim900a_check_cmd("+CLIP:");
				p += 8;
				p2 = (u8*)strstr((const char*)p,"\"");
				p2[0]=0;  //添加结束符
				strcpy((char*)p1,(char*)p);
			}
			USART3_RX_STA = 0;
		}
	
		key = KEY_Scan(0);
		if(key == 1)
		{
			printf("请拨号\r\n");
			LED1 = 0;
			if(cmode == 0)	 //拨号模式
			{
					if(USART_RX_STA & 0x8000)
					{
						memset((char*)phonenumber,0,sizeof((char*)phonenumber));
						len = USART_RX_STA & 0x3FFF; //得到此次接收到数据的长度
						for(i=0;i<len;i++)
						{
							phonenumber[i] = USART_RX_BUF[i];
						}  
						
						cmode = 1;		//拨号中
						USART_RX_STA = 0;
						delay_ms(20);  //一定要加延时，否则不能进行第二次接收
						printf("%s\r\n",phonenumber);
					}
				//u2_printf("ATD%s;\r\n",phonenumber);  //拨号	
			
				sprintf((char*)temp,"ATD%s;\r\n",phonenumber);   //使用send_cmd 拨号尝试
			   	sim900a_send_cmd("ATE1;","OK",200);
				sim900a_send_cmd(temp,"+COLP:",200);  
			  key = 0;
			  while(1)  //等待挂断
				{
					key = KEY_Scan(0);
					if(key == 3)		 //挂断电话
					{
						sim900a_send_cmd("ATH",0,0);
						LED1 = 1;
						break;
					}
				}
			}
				
		}
		if(cmode == 3)
		{
		//	key = KEY_Scan(0);
			if(key == 1)	  //接通电话
			{
				LED1 = 0;
				  sim900a_send_cmd("ATA","OK",200);		//发送应答指令
				  if(USART_RX_STA & 0x8000)
				{
					printf("%s",(char*)USART_RX_BUF);
					USART_RX_STA = 0;
					cmode=2;
				}
			}
		
			if(key == 3)	//挂断电话
			{
				sim900a_send_cmd("ATH",0,0);
				LED0 = 1;
				LED1 = 1;
			}
		}
	}
}

/////////////////////////////////////////GPRS////////////////////////////////////////////////////////// 
const u8 *modetbl[2] = {"TCP","UDP"};  	//模式连接
//tcp/udp测试
//带心跳功能,以维持连接
//mode:0:TCP测试;1,UDP测试)
//ipaddr:ip地址
//port:端口 

void sim900a_tcpudp_test(u8 mode,u8 *ipaddr,u8 *port)
{
	u8 p[100],p1[100],*p2,*p3;
	u8 key;
	u16 timex = 0;
	u8 count = 0;
	const u8* cnttbl[3] = {"正在连接","连接成功","连接关闭"};
	u8 connectsta = 0;  	//0,正在连接;1,连接成功;2,连接关闭; 
	u8 hbeaterrcnt=0;			//心跳错误计数器,连续5次心跳信号无应答,则重新连接
	u8 oldsta=0XFF;
	printf("-----------------GPRS 测试------------------\r\n");
	if(mode) printf("---ATK-SIM900A UDP连接测试-----\r\n");
	else printf("---ATK-SIM900A TCP连接测试-----\r\n");
	printf("KEY_UP:退出测试  KEY_DONW:发送数据\r\n");
	sprintf((char*)p,"IP地址:%s,端口:%s",ipaddr,port);		
	USART3_RX_STA = 0;
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[mode],ipaddr,port);
	if(sim900a_send_cmd(p,"OK",50)) return;
	while(1)
	{
		key = KEY_Scan(0);
		if(key == KEY_UP)  //退出测试
		{
			sim900a_send_cmd("AT+CIPCLOSE","CLOSE OK",500);
			sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);
			break;
		}
		else if(key == KEY_RIGHT &&(hbeaterrcnt==0))
		{
			printf("数据发送中...");
			if(sim900a_send_cmd("AT+CIPSEND",">",500) == 0)
			{
				printf("CIPSEND DATA:%s\r\n",p1);
				u2_printf("%s\r\n",p1);
				delay_ms(10);
				if(sim900a_send_cmd((u8*)0x1A,"SEND OK",1000) == 0)  //最长等待10s
					printf("数据发送成功\r\n");
				else
					printf("数据发送失败\r\n");
				delay_ms(1000);
			}
		else
			sim900a_send_cmd((u8*)0x1B,0,0);  //ESC取消发送
		oldsta = 0xFF;
	}
	if((timex%20) == 0)
	{
		LED0 = !LED0;
		count++;
		if(connectsta==2 || hbeaterrcnt>8)  //连接中断，或者连续8次心跳没有正确发送，则重新连接
		{
			sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500); 	//关闭连接
			sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);	 	//关闭移动场景 
			sim900a_send_cmd(p,"OK",500);						//尝试重新连接
			connectsta = 0;
			hbeaterrcnt = 0;
		}
		sprintf((char*)p1,"ATK-SIM900A %s测试 %d  ",modetbl[mode],count);
		printf("\r\n%s\r\n",(char*)p1);
	}
	if(connectsta==0 && (timex%200)==0)   //连接还没建立的时候,每2秒查询一次CIPSTATUS.
	{
		sim900a_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态
		if(strstr((const char*)USART2_RX_BUF,"CLOSED"))
			connectsta = 2;
		if(strstr((const char*)USART2_RX_BUF,"CONNECT OK"))
			connectsta = 1;
	}
	if(connectsta==1 && timex>=600)  //连接正常的时候,每6秒发送一次心跳
	{
		timex = 0;
		if(sim900a_send_cmd("AT+CIPSEND",">",200) == 0)  //发送数据
		{
			sim900a_send_cmd((u8*)0x00,0,0);	//发送数据0x00
			delay_ms(20);
			sim900a_send_cmd((u8*)0x1A,0,0);	//CTRL+Z，结束发送数据，启动一次传输
		}
		else
			sim900a_send_cmd((u8*)0x1B,0,0);	//ESC，取消发送
		hbeaterrcnt++;
		printf("hbeaterrcn:%d\r\n",hbeaterrcnt); //调试用
	}
	delay_ms(10);
	if(USART3_RX_STA & 0x8000) 		//接收到一次数据
	{
		USART2_RX_BUF[USART3_RX_STA&0x7FFF] = 0;	//添加结束符
		if(hbeaterrcnt)
		{
			if(strstr((const char*)USART2_RX_BUF,"SEND OK"))
				hbeaterrcnt = 0;  //心跳正常
		}
		p2 = (u8*)strstr((const char*)USART2_RX_BUF,"+IPD");
		printf("收到的数据: %s\r\n",p2);

		USART3_RX_STA = 0;
	}
	if(oldsta != connectsta)
	{
		oldsta = connectsta;
		printf("更新状态:%s\r\n",cnttbl[connectsta]);
	}
	timex++;
	}
}



//sim900a GPRS测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 sim900a_gprs_test(void)
{
	u8 port[5];//端口固定为8086,当你的电脑8086端口被其他程序占用的时候,请修改为其他空闲端口			
	u8 mode=0;				//0,TCP连接;1,UDP连接
	u8 key;
	u16 len;
	u16 i;
	u8 timex=0; 
	u8 ipbuf[16]; 		//IP缓存
	u8 iplen=0;			//IP长度 
	printf("连接方式:%s,端口:%s\r\n",(char*)modetbl[mode],(char*)port);
   	sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",100);	//关闭连接
	sim900a_send_cmd("AT+CIPSHUT","SHUT OK",100);		//关闭移动场景 
	if(sim900a_send_cmd("AT+CGCLASS=\"B\"","OK",1000))return 1;				//设置GPRS移动台类别为B,支持包交换和数据交换 
	if(sim900a_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))return 2;//设置PDP上下文,互联网接协议,接入点等信息
	if(sim900a_send_cmd("AT+CGATT=1","OK",500))return 3;					//附着GPRS业务
	if(sim900a_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500))return 4;	 	//设置为GPRS连接模式
	if(sim900a_send_cmd("AT+CIPHEAD=1","OK",500))return 5;	 				//设置接收数据显示IP头(方便判断数据来源)
	ipbuf[0]=0;
	printf("按KEY_UP可以切换连接模式\r\n"); 
	while(1)
	{
		
		key = KEY_Scan(0);
		if(key == KEY_UP)
		{
			mode = !mode;
			printf("切换到模式:%s\r\n",(char*)modetbl[mode]);
		}
		printf("按输入连接服务器IP\r\n");	
		while(1)
		{  
		
			if(USART_RX_STA & 0x8000)
			{
				memset((char*)ipbuf,0,sizeof((char*)ipbuf)); //清空字符串
				len=USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
				for(i=0;i<len;i++)	   //将BUF中的数据考到字符串中 利用strcpy会出错
				{
					ipbuf[i] = USART_RX_BUF[i];
				}
				USART_RX_STA = 0;
				printf("服务器地址:%s\r\n",(char*)ipbuf);
				printf("\r\n");
				delay_ms(20);  //一定要加延时，否则不能进行第二次接收
				break;
			}
		}
		printf("按输入连接服务器端口号\r\n");	
		while(1)
		{  
		
			if(USART_RX_STA & 0x8000)
			{
				memset((char*)port,0,sizeof((char*)port)); //清空字符串
				len=USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
				for(i=0;i<len;i++)	   //将BUF中的数据考到字符串中
				{
					port[i] = USART_RX_BUF[i];
				}
				USART_RX_STA = 0;
				printf("服务器端口号:%s\r\n",(char*)port);
				printf("\r\n");
				delay_ms(20);
				break;
			}	 
		}	 
		sim900a_tcpudp_test(mode,ipbuf,(u8*)port);	
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		sim_at_response(1);//检查GSM模块发送过来的数据,及时上传给电脑
	}
	return 0;
}			

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//短信测试部分代码

//SIM900A读短信测试
void sim900a_sms_read_test(void)
{
	u8 p[200],*p1,*p2;
	u8 timex = 0;
	u16 len,i;
	u8 msgindex[3];
	u8 msgchoose[3];
	u8 msgmaxnum = 0;     //短信最大条数
	u8 key = 0;
	u8 smsreadsta = 0;    //是否在短信显示状态
	memset((char*)msgindex,0,3);  //清空数组
	if(sim900a_send_cmd("AT+CNMI=2,1","OK",200)==0)  //设置消息提醒
		printf("新消息提醒设置成功\r\n");
	else
		printf("新消息提醒设置失败\r\n");	
	printf("---------读短信测试-----------\r\n");
	printf("按左按键读短信\r\n");
	while(1)
	{
		
		if(sim900a_check_cmd("+CMTI:"))
		{	 
			printf("收到新短信\r\n");
			p1 = (u8*)strstr((const char*)(USART2_RX_BUF),"\",");
			strcpy((char*)msgindex,(char*)p1);
			USART3_RX_STA = 0;
		} 
		key = KEY_Scan(0);
		if(key)
		{
			if(smsreadsta)
			{
				smsreadsta = 0;
			}
			if(key == KEY_LEFT)   //按左按键读取短信
			{
				printf("请输入要阅读的短信（小于%s条)\r\n",msgindex);	
				while(1)
				{  
				
					if(USART_RX_STA & 0x8000)
					{
						memset((char*)msgchoose,0,3);
						len=USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
						for(i=0;i<len;i++)	   //将BUF中的数据考到字符串中
						{
							msgchoose[i] = USART_RX_BUF[i];
						}
						printf("msgchoose=%s",(char*)msgchoose);
						USART_RX_STA = 0;
						delay_ms(20);
						break;
					}	 
				}	 
				sprintf((char*)p,"AT+CMGR=%s",msgchoose);
				if(sim900a_send_cmd(p,"+CMGR:",200)==0)    //发指令读取短信
				{
					len=USART3_RX_STA&0x7FFF;//得到此次接收到的数据长度 
					if(strstr((const char*)(USART2_RX_BUF),"UNREAD"))
						printf("未读短信：\r\n");
					printf("unicode字符：%s\r\n",USART2_RX_BUF);
					p1 = (u8*)strstr((const char*)(USART2_RX_BUF),"\",");	 //获取发信人号码
					p2 = (u8*)strstr((const char*)(p1+2),"\",");
					p2[0] = 0;  //加结束符
					printf("来自:%s\r\n",p1+3);
						p1 = (u8*)strstr((const char*)(p1+1),"/");   //这种表达不可取p1不能到p1
					p1 = (u8*)strstr((const char*)(p2+1),"/");//寻找接收时间字符位置
					p2 = (u8*)strstr((const char*)p1,"+");
					p2[0] = 0;   //添加结束符
					printf("接收时间:%s\r\n",p1-2);
				/*	p1 = (u8*)strstr((const char*)p2,"\r");
					sim900a_unigbk_exchange(p1+2,p,0);			//将unicode字符转换为gbk码
					printf("短信内容:%s\r\n",p);*/			
					smsreadsta = 1;
				}
				else
				{
				 	printf("无短信内容，请检查!\r\n");
					delay_ms(100);
				}
				USART3_RX_STA = 0;
			}
		}
			if(timex == 0)
			{
				if(sim900a_send_cmd("AT+CPMS?","+CPMS:",200)==0)  //查询优选消息存储器
				{
					p1 = (u8*)strstr((const char*)(USART2_RX_BUF),","); 
					p2 = (u8*)strstr((const char*)(p1+1),",");
					p2[0] = 0;
					sprintf((char*)msgindex,"%s",p1+1);
					printf("可阅读短信有%s条\r\n",msgindex);
					p2[0] = '/'; 
					if(p2[3]==',')	//小于64K SIM卡，最多存储几十条短信
					{
						msgmaxnum = (p2[1]-'0')*10+p2[2]-'0';		//获取最大存储短信条数
						p2[3] = 0;
					}else //如果是64K SIM卡，则能存储100条以上的信息
					{
						msgmaxnum = (p2[1]-'0')*100+(p2[2]-'0')*10+p2[3]-'0';//获取最大存储短信条数
						p2[4] = 0;
					}
					sprintf((char*)p,"%s",p1+1);
					printf("使用空间%s",p);
					USART3_RX_STA=0;
				}
			}	
			if((timex%20)==0)LED0=!LED0;//200ms闪烁 
			timex++;
			delay_ms(10);
			if(USART3_RX_STA&0X8000)sim_at_response(1);//检查从GSM模块接收到的数据 
		}
}
	

//测试短信发送内容(70个字[UCS2的时候,1个字符/数字都算1个字])
const u8* sim900a_test_msg = "ATK_SIM900A TEST messeage";
//SIM900A发短信测试

void sim900a_sms_send_test(void)
{
	u8 p[100],p1[300],p2[100];
			//p:用于存放电话号码的unicode字符串
			//p1:用于存放短信的unicode字符串
			//p2: 存放：AT+CMGS=p1 
	u8 phonebuf[20];	  //号码缓存
	u8 phonenumlen = 0;	  //号码长度，最大15个数
	u8 timex = 0;
	u8 key = 0;
	u16 len,i;
	u8 smssendsta = 0; 	  //短信发送状态,0,等待发送;1,发送失败;2,发送成功
	printf("-----发短信测试------\r\n");
	printf("短信内容: %s",sim900a_test_msg);
	printf("按左按键进入发短信模式,按右键退出测试\r\n");
	while(1)
	{
		key = KEY_Scan(0);
		if(key)
		{
			if(smssendsta)
			{
				smssendsta = 0;
				printf("等待发送\r\n");
			}
			if(key ==KEY_LEFT)
			{
				printf("请输入号码\r\n");
				while(1)	//获取号码
				{  
			
					if(USART_RX_STA & 0x8000)
					{
						memset((char*)phonebuf,0,sizeof((char*)phonebuf)); //清空字符串
						len=USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
						for(i=0;i<len;i++)	   //将BUF中的数据考到字符串中 利用strcpy会出错
						{
							phonebuf[i] = USART_RX_BUF[i];
						}
						USART_RX_STA = 0;
						delay_ms(20);  //一定要加延时，否则不能进行第二次接收
						break;
					}
				}
			   	printf("按左按键执行发送，按右按键结束发送\r\n");
				while(1)
				{
					key = KEY_Scan(0);
					if(key == KEY_LEFT)
					{
						printf("正在发送...\r\n");
						smssendsta = 1;
						sprintf((char*)p2,"AT+CMGS=\"%s\"",phonebuf);
						if(sim900a_send_cmd(p2,">",200) == 0)  //号码发送成功，
						{
							u2_printf("%s",sim900a_test_msg);
							if(sim900a_send_cmd((u8*)0x1A,"+CMGS:",1000)==0)
								smssendsta = 2;
						}
						if(smssendsta ==1)
							printf("发送失败！\r\n");
						else
							printf("发送成功！\r\n");
						USART3_RX_STA = 0;
					}
					if(key == KEY_RIGHT)
						break;
				}
			}
		}
		if((timex%20)==0)LED0=!LED0;//200ms闪烁 
		timex++;
		delay_ms(10);
		if(USART3_RX_STA&0X8000)sim_at_response(1);//检查从GSM模块接收到的数据
		if(key == KEY_RIGHT)
			break;  //退出测试
	}
}

//sim900a短信测试
//用于读短信或者发短信
//返回值:0,正常
//    其他,错误代码
u8 sim900a_sms_test(void)
{
	u8 key;
	u8 timex=0;
	//if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 1;			//设置文本模式 
//	if(sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",200))return 2;	//设置TE字符集为UCS2 
//	if(sim900a_send_cmd("AT+CSMP=17,0,2,25","OK",200))return 3;	//设置短消息文本模式参数 
   	//纯英文短信发送
   	if(sim900a_send_cmd("AT+CSCS=\"GSM\"","OK",200))return 2;	//设置TE字符集为UCS2 
	if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 1;			//设置文本模式
	
	printf("按右键读短信测试，按左键发短信测试，按上键退出测试\r\n");
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_RIGHT)
		{ 
			sim900a_sms_read_test();
			timex=0;
		}else if(key==KEY_DOWN)
		{ 
			sim900a_sms_send_test();
			timex=0;			
		}else if(key==KEY_UP)break;
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		sim_at_response(1);										//检查GSM模块发送过来的数据,及时上传给电脑
	} 
	//sim900a_send_cmd("AT+CSCS=\"GSM\"","OK",200);				//设置默认的GSM 7位缺省字符集
	return 0;
}

//GSM信息显示(信号质量,电池电量,日期时间)
//返回值:0,正常
//    其他,错误代码
u8 sim900a_gsminfo_show(u16 x,u16 y)
{
	u8 p[50],*p1,*p2;
	u8 res=0;	
	USART3_RX_STA=0;
	if(sim900a_send_cmd("AT+CPIN?","OK",200))res|=1<<0;	//查询SIM卡是否在位 
	USART3_RX_STA=0;  
	if(sim900a_send_cmd("AT+COPS?","OK",200)==0)		//查询运营商名字
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\""); 
		if(p1)//有有效数据
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");
			p2[0]=0;//加入结束符			
			sprintf((char*)p,"运营商:%s",p1+1);
			printf("%s",p);
		} 
		USART3_RX_STA=0;		
	}else res|=1<<1;
	if(sim900a_send_cmd("AT+CSQ","+CSQ:",200)==0)		//查询信号质量
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;//加入结束符
		sprintf((char*)p,"信号质量:%s",p1+2);
		printf("%s",p);
		USART3_RX_STA=0;		
	}else res|=1<<2;
	if(sim900a_send_cmd("AT+CBC","+CBC:",200)==0)		//查询电池电量
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+1),",");
		p2[0]=0;p2[5]=0;//加入结束符
		sprintf((char*)p,"电池电量:%s%%  %smV",p1+1,p2+1);
		printf("%s",p);
		USART3_RX_STA=0;		
	}else res|=1<<3; 
	if(sim900a_send_cmd("AT+CCLK?","+CCLK:",200)==0)		//查询电池电量
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\"");
		p2=(u8*)strstr((const char*)(p1+1),":");
		p2[3]=0;//加入结束符
		sprintf((char*)p,"日期时间:%s",p1+1);
		printf("%s",p);
		USART3_RX_STA=0;		
	}else res|=1<<4; 
	return res;
} 

//sim900a主测试程序
void sim900a_test(void)
{
	u8 key=0; 
	u8 timex=0;
	u8 sim_ready=0;
	printf("%s\r\n","ATK-SIM900A 测试程序"); 
	while(sim900a_send_cmd("AT","OK",100))//检测是否应答AT指令 
	{
		printf("%s\r\n","未检测到模块!!!");
		delay_ms(800);
	
		printf("%s\r\n","尝试连接模块...");
		delay_ms(400);  
	} 
	key+=sim900a_send_cmd("ATE0","OK",200);//不回显	
	while(1)
	{
		delay_ms(10); 
		sim_at_response(1);//检查GSM模块发送过来的数据,及时上传给电脑
		printf("按右键读拨号测试，按下键短信测试，按上键GPRS测试\r\n");
		if(sim_ready)//SIM卡就绪.
		{
			key=KEY_Scan(0); 
			if(key)
			{
				switch(key)
				{
					case KEY_RIGHT:
						sim900a_call_test();//拨号测试
						break;
					case KEY_DOWN:
						sim900a_sms_test();	//短信测试
						break;
					case KEY_UP:
						sim900a_gprs_test();//GPRS测试
						break;
				}
				timex=0;
			} 			
		}
		if(timex==0)		//2.5秒左右更新一次
		{
			if(sim900a_gsminfo_show(40,225)==0)sim_ready=1;
			else sim_ready=0;
		}	
		if((timex%20)==0)LED0=!LED0;//200ms闪烁 
		timex++;	 
	} 	
}
		

