#ifndef __LED_H
#define __LED_H
#define LED_OFF GPIO_SetBits(GPIOB,GPIO_Pin_0)   //�رյ�
#define LED_ON GPIO_ResetBits(GPIOB,GPIO_Pin_0) //������
void LED_Init(void);
#endif
