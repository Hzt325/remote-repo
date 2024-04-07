#include "sys.h"
#include "usart.h"	
/////////////////////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
///////////////////////////////////////////////////////////////////////////////
int bluetooth=9;
extern float velocity_Rcalcu; //��ʵ�ٶ�����ֵ	
extern float yaw_calcu;       //yaw������ֵ
int Serial_RxPacket[]={0,0,0,0,0,0};
int Trace[]={0,0,0,0,0,0,0,0};
int stop_num;

void usart2_init(u32 bound)
{
	//GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			          //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ��GPIOA.2
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;               //PA.3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ��GPIOA.3 

  //Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_InitStructure.USART_BaudRate = bound;                     //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;             //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure);      //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //��������2�����ж�
  USART_Cmd(USART2, ENABLE);                     //ʹ�ܴ���2

}

//����2�жϷ������
void USART2_IRQHandler(void)          	
{
	static unsigned char RxState = 0;	
  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    // �����жϷ���
		Serial_RxPacket[RxState++] = USART_ReceiveData(USART2);
    // ������յ�������
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
	
  // ʹ��USART3��GPIOB����ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);



  // ����USART3����
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // USART3 TX����
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // GPIO�ٶ�
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11; // USART3 RX����
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
  // �����ж����ȼ�
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  // ����USART3����
  USART_InitStruct.USART_BaudRate = 115200; // ������Ϊ9600
  USART_InitStruct.USART_WordLength = USART_WordLength_8b; // ����λ����Ϊ8λ
  USART_InitStruct.USART_StopBits = USART_StopBits_1; // ֹͣλΪ1λ
  USART_InitStruct.USART_Parity = USART_Parity_No; // ����żУ��
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ��������
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // ֧�ֽ��պͷ���
  USART_Init(USART3, &USART_InitStruct);

  // ʹ��USART3�����ж�
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);


  // ʹ��USART3
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
		bluetooth = USART_ReceiveData(USART2); //ͨ������2��������   	
		//��1.��ת��
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
		
		//��2.��ǰ���ܡ�
		if(bluetooth==3) velocity_Rcalcu=0.073; //��ÿ��
		if(bluetooth==4) velocity_Rcalcu=-0.073;
		if(bluetooth==9) velocity_Rcalcu=0;
		
		// ����жϱ�־
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  } 
}

