#ifndef __XUNJI_H
#define __XUNJI_H

#define xunji_1 GPIO_Pin_6
#define xunji_2 GPIO_Pin_7
#define xunji_3 GPIO_Pin_8
#define xunji_4 GPIO_Pin_9
#define xunji_5 GPIO_Pin_10

void Xunji_Init(void);

uint8_t Read_Xunji(uint16_t GPIO_Pin);
#endif
