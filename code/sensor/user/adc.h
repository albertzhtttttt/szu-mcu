/*********************************************
*文件名：adc.h                            **
*功能：ADC配置相关操作头文件				**
*作者：czz                                  **
*修改记录：									**						
		2016\10\22		创建				**
*********************************************/

#ifndef ADC_H
#define ADC_H

/********************************************/

/**************包含的头文件******************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f10x.h"


/************* 函数调用声明 *************/

void ADC1_GPIO_Conf(void );
void ADC1_Mode_Conf(void);
void ADC1_Init(void );

#endif

