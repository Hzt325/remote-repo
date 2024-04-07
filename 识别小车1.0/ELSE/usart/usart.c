#include "sys.h"
#include "usart.h"	
/////////////////////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
///////////////////////////////////////////////////////////////////////////////
int bluetooth=9;
extern float velocity_Rcalcu; //真实速度理论值	
extern float yaw_calcu;       //yaw角理论值
int Serial_RxPacket[]={0,0,0,0,0,0};
int Trace[]={0,0,0,0,0,0,0,0};
int stop_num;

void usart2_init(u32 bound)
{
	//GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			          //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化GPIOA.2
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;               //PA.3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化GPIOA.3 

  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器
	
	USART_InitStructure.USART_BaudRate = bound;                     //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;             //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure);      //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //开启串口2接受中断
  USART_Cmd(USART2, ENABLE);                     //使能串口2

}

//串口2中断服务程序：
void USART2_IRQHandler(void)          	
{
	static unsigned char RxState = 0;	
  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    // 接收中断发生
		Serial_RxPacket[RxState++] = USART_ReceiveData(USART2);
    // 处理接收到的数据
    // ...
		if(Serial_RxPacket[0]!=0xa3) RxState=0;
		if((RxState==2)&&(Serial_RxPacket[1]!=0xb3)) RxState=0;
		if((RxState==5)&&(Serial_RxPacket[4]!=0xc3)) RxState=0;
		if(RxState==5)
		{
			RxState=0;
			if(openmv_data_test(Serial_RxPacket))
			{
				Trace[0]=(Serial_RxPacket[2]&0x40)>>6;						
				Trace[1]=(Serial_RxPacket[2]&0x20)>>5;
				Trace[2]=(Serial_RxPacket[2]&0x10)>>4;
				Trace[3]=(Serial_RxPacket[2]&0x08)>>3;
				Trace[4]=(Serial_RxPacket[2]&0x04)>>2;
				Trace[5]=(Serial_RxPacket[2]&0x02)>>1;
				Trace[6]=(Serial_RxPacket[2]&0x01)>>0;
				
				stop_num=Serial_RxPacket[3];
			}
		}            

  }
	
} 


void USART3_Init(void)
{
		
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	
  // 使能USART3和GPIOB外设时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);



  // 配置USART3引脚
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // USART3 TX引脚
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11; // USART3 RX引脚
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
  // 配置中断优先级
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  // 配置USART3参数
  USART_InitStruct.USART_BaudRate = 115200; // 波特率为9600
  USART_InitStruct.USART_WordLength = USART_WordLength_8b; // 数据位长度为8位
  USART_InitStruct.USART_StopBits = USART_StopBits_1; // 停止位为1位
  USART_InitStruct.USART_Parity = USART_Parity_No; // 无奇偶校验
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 支持接收和发送
  USART_Init(USART3, &USART_InitStruct);

  // 使能USART3接收中断
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);


  // 使能USART3
  USART_Cmd(USART3, ENABLE);
}


int openmv_data_test(int data[])
{
	if(data[0]!=0xa3) return 0;
	if(data[1]!=0xb3) return 0;
	if(data[4]!=0xc3) return 0;
	
	return 1;
}

void USART3_IRQHandler(void)
{
if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
	{
		bluetooth = USART_ReceiveData(USART2); //通过串口2接收数据   	
		//【1.调转向】
		if(bluetooth==1) 
		{
			yaw_calcu=yaw_calcu-10;
			if(yaw_calcu<-160) yaw_calcu=-160;
		}
		if(bluetooth==2) 
		{
			yaw_calcu=yaw_calcu+10;
			if(yaw_calcu>160) yaw_calcu=160;
		}
		
		//【2.调前后跑】
		if(bluetooth==3) velocity_Rcalcu=0.073; //米每秒
		if(bluetooth==4) velocity_Rcalcu=-0.073;
		if(bluetooth==9) velocity_Rcalcu=0;
		
		// 清除中断标志
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  } 
}

